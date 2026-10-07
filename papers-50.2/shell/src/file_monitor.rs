use super::*;
use std::cell::Cell;

mod imp {
    use super::*;

    #[derive(Properties, Default, Debug)]
    #[properties(wrapper_type = super::PpsFileMonitor)]
    pub struct PpsFileMonitor {
        #[property(construct_only, get)]
        pub(super) uri: RefCell<String>,
        pub(super) monitor: RefCell<Option<gio::FileMonitor>>,
        pub(super) timeout_id: RefCell<Option<glib::SourceId>>,
        pub(super) suspend_count: Cell<u32>,
        pub(super) generation: Cell<u64>,
    }

    #[glib::object_subclass]
    impl ObjectSubclass for PpsFileMonitor {
        const NAME: &'static str = "PpsFileMonitor";
        type Type = super::PpsFileMonitor;
        type ParentType = glib::Object;
    }

    #[glib::derived_properties]
    impl ObjectImpl for PpsFileMonitor {
        fn signals() -> &'static [Signal] {
            static SIGNALS: OnceLock<Vec<Signal>> = OnceLock::new();
            SIGNALS.get_or_init(|| vec![Signal::builder("changed").run_last().action().build()])
        }

        fn constructed(&self) {
            self.start();
        }

        fn dispose(&self) {
            self.stop();
        }
    }

    impl PpsFileMonitor {
        pub(super) fn start(&self) {
            let file = gio::File::for_uri(self.uri.borrow().as_ref());
            match file.monitor_file(gio::FileMonitorFlags::NONE, gio::Cancellable::NONE) {
                Ok(monitor) => {
                    monitor.connect_changed(glib::clone!(
                        #[weak(rename_to = obj)]
                        self,
                        move |monitor, _, _, event| {
                            // Events already queued by a cancelled monitor must not reload
                            // a document after its own save has finished.
                            if monitor.is_cancelled() {
                                return;
                            }
                            match event {
                                gio::FileMonitorEvent::ChangesDoneHint => {
                                    obj.timeout_stop();
                                    obj.obj().emit_by_name::<()>("changed", &[]);
                                }
                                gio::FileMonitorEvent::Changed => obj.timeout_start(),
                                _ => (),
                            }
                        }
                    ));

                    self.monitor.replace(Some(monitor));
                }
                Err(e) => {
                    glib::g_warning!("", "{}", e.message());
                }
            }
        }

        pub(super) fn stop(&self) {
            self.timeout_stop();
            if let Some(monitor) = self.monitor.take() {
                monitor.cancel();
            }
        }
        fn timeout_start(&self) {
            self.timeout_stop();

            let id = glib::timeout_add_seconds_local_once(
                5,
                glib::clone!(
                    #[weak(rename_to = obj)]
                    self,
                    move || {
                        obj.timeout_id.take();
                        obj.obj().emit_by_name::<()>("changed", &[]);
                    }
                ),
            );

            self.timeout_id.replace(Some(id));
        }

        fn timeout_stop(&self) {
            if let Some(id) = self.timeout_id.take() {
                id.remove();
            }
        }
    }
}

glib::wrapper! {
    pub struct PpsFileMonitor(ObjectSubclass<imp::PpsFileMonitor>);
}

impl PpsFileMonitor {
    pub fn new(uri: &str) -> Self {
        glib::Object::builder().property("uri", uri).build()
    }

    pub fn is_suspended(&self) -> bool {
        self.imp().suspend_count.get() > 0
    }

    pub fn generation(&self) -> u64 {
        self.imp().generation.get()
    }

    pub fn suspend(&self) -> FileMonitorGuard {
        let imp = self.imp();
        imp.suspend_count.set(imp.suspend_count.get() + 1);
        imp.generation.set(imp.generation.get().wrapping_add(1));
        imp.stop();
        FileMonitorGuard(self.clone())
    }
}

// Keep the monitor suspended until the save callback is disconnected, including
// error paths. A fresh monitor cannot receive notifications from the old write.
pub struct FileMonitorGuard(PpsFileMonitor);

impl Drop for FileMonitorGuard {
    fn drop(&mut self) {
        let imp = self.0.imp();
        let count = imp.suspend_count.get() - 1;
        imp.suspend_count.set(count);
        if count == 0 {
            imp.start();
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::rc::Rc;
    use std::time::{Duration, Instant};

    fn pump_for(duration: Duration) {
        let context = glib::MainContext::default();
        let until = Instant::now() + duration;
        while Instant::now() < until {
            while context.pending() {
                context.iteration(false);
            }
            std::thread::sleep(Duration::from_millis(5));
        }
    }

    #[test]
    fn own_write_and_queued_events_are_ignored_but_external_writes_resume() {
        let context = glib::MainContext::default();
        let _context = context.acquire().unwrap();
        let path = std::env::temp_dir().join(format!("papers-monitor-{}", std::process::id()));
        std::fs::write(&path, b"original").unwrap();
        let file = gio::File::for_path(&path);
        let monitor = PpsFileMonitor::new(&file.uri());
        let changes = Rc::new(Cell::new(0));
        monitor.connect_closure(
            "changed",
            false,
            glib::closure_local!(
                #[strong]
                changes,
                move |_: PpsFileMonitor| changes.set(changes.get() + 1)
            ),
        );

        for _ in 0..3 {
            let old = monitor.imp().monitor.borrow().clone().unwrap();
            let generation = monitor.generation();
            let guard = monitor.suspend();
            let nested_guard = monitor.suspend();
            assert!(old.is_cancelled());
            std::fs::write(&path, b"own save").unwrap();
            pump_for(Duration::from_millis(300));
            assert_eq!(changes.get(), 0);
            drop(guard);
            assert!(monitor.is_suspended());
            drop(nested_guard);
            assert!(!monitor.is_suspended());
            assert_ne!(monitor.generation(), generation);

            // Deliver a notification from the old monitor after save completion.
            old.emit_by_name::<()>(
                "changed",
                &[
                    &file,
                    &Option::<gio::File>::None,
                    &gio::FileMonitorEvent::ChangesDoneHint,
                ],
            );
            pump_for(Duration::from_millis(300));
            assert_eq!(changes.get(), 0);
        }

        std::fs::write(&path, b"external update after save").unwrap();
        let deadline = Instant::now() + Duration::from_secs(3);
        while changes.get() == 0 && Instant::now() < deadline {
            pump_for(Duration::from_millis(20));
        }
        assert!(changes.get() > 0, "external updates must still be detected");
        monitor.imp().stop();
        std::fs::remove_file(path).unwrap();
    }

    #[test]
    fn suspending_cancels_the_pending_reload_timeout() {
        let context = glib::MainContext::default();
        let _context = context.acquire().unwrap();
        let path = std::env::temp_dir().join(format!("papers-timeout-{}", std::process::id()));
        std::fs::write(&path, b"original").unwrap();
        let file = gio::File::for_path(&path);
        let monitor = PpsFileMonitor::new(&file.uri());
        let changes = Rc::new(Cell::new(0));
        monitor.connect_closure(
            "changed",
            false,
            glib::closure_local!(
                #[strong]
                changes,
                move |_: PpsFileMonitor| changes.set(changes.get() + 1)
            ),
        );
        monitor
            .imp()
            .monitor
            .borrow()
            .as_ref()
            .unwrap()
            .emit_by_name::<()>(
                "changed",
                &[
                    &file,
                    &Option::<gio::File>::None,
                    &gio::FileMonitorEvent::Changed,
                ],
            );
        assert!(monitor.imp().timeout_id.borrow().is_some());
        drop(monitor.suspend());
        assert!(monitor.imp().timeout_id.borrow().is_none());
        pump_for(Duration::from_secs(6));
        assert_eq!(changes.get(), 0);
        monitor.imp().stop();
        std::fs::remove_file(path).unwrap();
    }
}
