# In-place save refreshed the reading view, 2026-10-07

The Linux `inplace-save` branch wrote the PDF through `JobSave`, while
`PpsFileMonitor` continued observing that same file. A completed-write
notification immediately requested a reload; an incomplete notification could
instead request one through the five-second fallback timer. Once the save
cleared the modified state, the window could reload without asking.

Reloading replaces the document model and clears/rebuilds the page widgets and
render caches. It therefore refreshes the reading view and can affect the scroll
position. The user's original occasional page jump was not captured in a live
session; the unnecessary document replacements were reproduced in a test.

## Change

- Suspend monitoring before a save job writes over the monitored file. Cancel
  its pending timeout and underlying GIO monitor, and reject queued events from
  the cancelled monitor.
- Retain a guard through the finished callback, including save errors. On its
  release, create a fresh monitor immediately. There is no arbitrary period
  during which later external changes are ignored.
- Cancel an already-running reload. Tag queued reload requests with the monitor
  generation so they cannot run after a save or a different document supersedes
  them.
- Keep one save active. Job cancellation alone does not interrupt a worker
  already copying the output file; repeated Ctrl+S must not prematurely release
  the monitor guard.
- Apply the guard to Save As when its selected target is the monitored file.
  Saving a different copy leaves monitoring active.

The existing document, page widgets, zoom, page number, and scroll adjustments
are retained during in-place saving. External changes after completion still
follow the existing reload behavior.

## Regression checks

`scripts/build-shell.sh test` exercises these paths on disposable local files:

1. Three writes while suspended, nested guards, late events from old monitors,
   and detection of a subsequent real external write.
2. Cancellation of a pending five-second reload timer, waiting past its deadline.
3. A live GTK window displaying an eight-page generated PDF. The test moves to
   the middle, scrolls within the page, adds and saves three annotations, queues
   a reload immediately before each save, and repeats the save action while a
   job is active. It checks zero document replacements, identical page and
   scroll position, and the saved annotations after reopening. It also forces
   a copy failure by replacing the disposable target with a directory, then
   verifies monitoring resumes and a subsequent external write reloads.

The control run disabled the save guards, retaining the same harness. After the
first save it observed two document replacements and failed the assertion that
own saves must not replace the document. The guarded run passes all three tests.

The existing three Meson programs also pass: annotation lifecycle, save/reopen
cycles, and Chinese FreeText render/save. Five installer tests pass. Headless
GTK emitted layout/metadata warnings in both control and fixed runs.

## Build and delivery

The updated shell is compiled in release mode against the installed fixed
Papers libraries, using GNOME SDK 50 and the existing Rust extension. Exporting
copies that installation and replaces only the executable, preserving the
September library and font fixes. The deb version is `50.2+fix20261007-1`.

The installed/exported Flatpak commit is
`7f440512d1e8e2cd7e0065f1cda7c29f60079cf9c9db0e6a0b671edd8d3b90bb`.
The installed executable SHA-256 is
`bf94ba246b3b256fcdac2205da6e25c663305056199f4799c79faad5477b6b93`.
The view library, document library, and PDF backend hashes match those in the
still-running September installation. The installed shell's version command
and runtime linkage succeed; the extracted deb payload and pinned launcher
match the export inputs. The new source patch applies cleanly and reproduces
the three changed shell source files.

Tests run on a separate Broadway display with a private D-Bus session and
in-memory settings. Existing user windows are not closed. They continue using
their original executable until the user saves their work and reopens them.

This validates the application save action and local GIO notifications. Full
keyboard replay, real OneDrive round trips, simultaneous writes from another
application during a save, and every filesystem/backend are not covered.
