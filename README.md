# Papers: in-place save and annotation fixes

Personal Linux build of GNOME Papers 50.2. Keeps **selected text readable** and **Ctrl+S saving to the current file without refreshing the reading view**, and fixes annotation deletion crashes and Chinese FreeText disappearing outside editing mode.

This is an independent downstream project, not an official GNOME release. The deb is a **per-user Flatpak installer**, not a native build against Ubuntu's GTK libraries.

## Install

Download the deb and SHA256SUMS from Releases, then:

```sh
sudo apt install ./papers-inplace-save_50.2+fix20261028-1_amd64.deb
papers-inplace-save --install
```

Run the second command as your normal desktop user, without sudo. Alternatively open **Papers (in-place save fixes)** from the application menu. The first launch installs the bundled Flatpak for the current user. GNOME Platform 50 must be available; Flatpak may download it. The launcher keeps application data and does not close existing windows. Preserve unsaved work and completely exit old Papers windows before reopening.

A standalone `papers-fixed.flatpak` is also provided:

```sh
flatpak install --user --or-update ./papers-fixed.flatpak
flatpak run --user org.gnome.Papers//inplace-save
```

The deb installs a launcher and payload only: it does not run a root Flatpak installation, change the default PDF handler, or remove user application data. Removing the deb does not uninstall the per-user Flatpak. To remove that separately, run `flatpak uninstall --user org.gnome.Papers//inplace-save` (do not add `--delete-data` if retaining settings).

## Changes

- Detect Poppler's transparent-selection support when rebuilding libview, matching the installed PDF backend. Without the matching build flag, the backend drew an opaque accent-colored rectangle over selected text. Focused selections now use 30% opacity and unfocused selections use at most 10%; region-only selection overlays also remain translucent.
- Ignore deleted or stale annotation objects before modifying the PDF backend or undo history.
- Cancel deferred empty-text deletion when the widget is disposed; recheck contents and deletion state before executing.
- When the selected font lacks a character and Pango finds a single fallback covering the whole text, use that font for the PDF annotation. Preserve point size and keep the original font when it covers the text. This also repairs font selection when an existing annotation enters editing mode.
- Preserve the earlier Rust shell changes implementing Ctrl+S in-place saving.
- Bind Ctrl+S to the in-place save action in the compiled UI; reserve Ctrl+Shift+S for Save As. Verify these bindings through the live GTK shortcut controllers during save regression checks.
- Suspend file monitoring while saving over the open file, including Save As to the same target. Restore monitoring on completion or failure, and discard queued notifications from the old monitor. Ctrl+S retains the document, current page, and scroll position; subsequent external edits still reload normally.
- Keep one save job active so repeated Ctrl+S cannot cancel a running write and restore monitoring too soon.

No single fallback covering a mixed-script string means the existing font is retained. This does not solve every shaping, embedding, or cross-machine font issue.

## Source and build

`papers-50.2/` contains the full Papers source plus the patches. The upstream archive SHA-256 is `ae1bdcf1cd47cb50c9d84765784607f81c72df17dd6e6ad933fea14173d2b9f4` from `https://download.gnome.org/sources/papers/50/papers-50.2.tar.xz`. Patches are also available separately. GPL-2.0-or-later applies; retain upstream notices (see COPYING and source headers).

The current release rebuilds `libppsview` and the Rust shell, retaining the installed PDF backend and dependencies. The root Meson project rebuilds the library against the installed Flatpak and GNOME SDK 50, using the required checked-in generated resources from the original build. Its feature detection links to the installed Poppler library because the deployment does not include Poppler development headers:

```sh
flatpak install flathub org.gnome.Sdk//50
scripts/build-view.sh test
```

Build and test the updated shell against the installed `inplace-save` branch:

```sh
scripts/build-shell.sh test
scripts/build-shell.sh build
scripts/export-flatpak.sh
```

The scripts use GNOME SDK 50 and its Rust extension. GTK tests run on separate Broadway displays and private D-Bus sessions with in-memory settings; they do not touch existing windows or user PDFs. The export script copies the installed application, replaces `papers` and `libppsview`, exports a new Flatpak, updates the launcher's pinned commit, and builds the deb in `dist/`.

`packaging/org.gnome.Papers.json` retains the original pinned dependency sources and adds the four patches. It can be used with `flatpak-builder --user --install --default-branch=inplace-save --force-clean staging packaging/org.gnome.Papers.json` for a full rebuild. **That full rebuild has not been executed for this release.** Runtime libraries are supplied by GNOME Platform 50.

Build the deb from the release Flatpak:

```sh
scripts/build-deb.sh /path/to/papers-fixed.flatpak
```

The installer pins the shipped Flatpak commit. A new export requires updating `expected_commit` in the launcher before rebuilding the deb.

## Validation

The selection regression generates a disposable PDF containing English and Chinese, checks text extraction, and exercises background selection rendering, drag updates, zoom, loss of focus, and clearing the selection. The old build produced a fully opaque overlay (255/255 alpha) and failed; the fixed build stays at or below 30% opacity. All four Meson programs, three shell regressions, and five installer checks pass.

Eight lifecycle checks pass. Against old source, six stale/deferred-removal cases fail and two normal-operation cases pass; all eight pass after the fix. Chinese editing/render/save/reopen fails with the old font behavior and passes after the fix. A separate test completes three in-place JobSave/reopen cycles, checks annotation counts, and rejects stale deletion. See [incident and validation record](doc/incident-20260922.md).

Tests use disposable PDFs on ordinary local storage. Full GUI keystroke replay, every font/script, and OneDrive upload/download end-to-end have not been verified. No original user document is distributed.

The shell regressions additionally exercise real file notifications, queued events, pending timeouts, repeated saves, and a live GTK reading view. See [save/refresh regression record](doc/save-refresh-20261007.md).

Disposable test output, obsolete result JSON files, and unused previewer resources have been removed. Keep the reusable regression sources and the generated files required by the library build. Build directories and `dist/` are ignored and can be removed after publishing; the scripts recreate them.
