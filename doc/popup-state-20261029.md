# Note popup reopens after being closed, 2026-10-29

Status: reproduced, fixed, packaged as `50.2+fix20261029-1`, and installed locally. Existing windows continue running the previous version until the user restarts them. The October 28 release artifacts remain available separately.

The reported private PDF has a text annotation on page 2 whose associated `/Popup` dictionary contains `/Open true`. Papers reads that value into `popup-is-open`, then initializes the annotation window's `is_open` from it. When the page becomes visible, `show_annotation_windows()` shows existing windows whose `is_open` is true. This explains why the popup appears during navigation.

The defect is in explicit closing: `pps_annotation_window_close_request()` set the window's `is_open` to false but left the annotation's `popup-is-open` true. Saving therefore retained `/Open true`, and reopening the PDF restored the popup. Opening an existing note already sets the annotation property true through `pps_view_handle_annotation()`.

The close handler now also calls `pps_annotation_markup_set_popup_is_open(..., FALSE)`. The existing backend notification updates Poppler and marks the document modified. Temporary hiding during navigation still leaves the state unchanged. PDFs that deliberately save an open popup continue to restore it until the user explicitly closes and saves it.

## Verification

- Before the fix, the new regression fails after `gtk_window_close()` because the annotation is still marked open. The four existing Meson programs pass.
- After the fix, `scripts/build-view.sh test` passes all five Meson programs, including lifecycle, save/reload, Chinese FreeText rendering, selection rendering, and popup state persistence.
- The popup regression verifies that temporary hiding preserves the open state without marking the document modified, explicit closing hides the window and updates the annotation, and saving/reopening preserves the note's count and contents with its popup closed.
- The same regression passes with the reported private PDF and the exact note selected by contents. It copies the input into a temporary directory before any modification; the source directory was exposed read-only to the Flatpak test process. No private PDF is included in the repository.
- The regression uses the public annotation editor entry point and dynamically links the view library. Explicitly preloading the previous installed library reproduces the failure; preloading `/app/lib/libppsview-4.0.so.5` from the updated installation passes the popup and save/reload regressions, including a temporary copy of the reported private PDF. The executable's legacy RPATH takes precedence over `LD_LIBRARY_PATH`, so `LD_PRELOAD` was used to select and verify the installed library.
- Five installer checks pass. The deb contains the matching Flatpak and pinned launcher, and the installed view library matches the stripped tested build. The Rust shell is byte-identical to the previous running application. No running application was stopped and no user PDF was modified by the installation or tests.

The installed Flatpak commit is `63c020700f95599717dfadc560eefd45bc86fc396943ed2a260da0e0e4a5c519`; its view library SHA-256 is `d131aa358d5962798b8d0219ca219518dada0bbf4d226523a79cb502f0b459f6`. This release rebuilt only the view library and retained the October 28 shell and existing dependencies. A complete rebuild from the manifest was not performed.

To check a particular note using the regression executable inside the isolated Broadway test environment, supply the source path followed by its exact contents:

```sh
build/test-annotation-popup /path/to/document.pdf 'exact note contents'
```

Without the final argument, the test creates a temporary note in a disposable copy of the fixture. The test exercises the GTK close request and the PDF backend's save/reopen path; desktop pointer/keyboard navigation and cloud synchronization were not replayed.
