# Papers: in-place save and annotation fixes

Personal Linux build of GNOME Papers 50.2. Keeps **Ctrl+S saving to the current file** and fixes annotation deletion crashes and Chinese FreeText disappearing outside editing mode.

This is an independent downstream project, not an official GNOME release. The deb is a **per-user Flatpak installer**, not a native build against Ubuntu's GTK libraries.

## Install

Download the deb and SHA256SUMS from Releases, then:

```sh
sudo apt install ./papers-inplace-save_50.2+fix20260922-1_amd64.deb
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

- Ignore deleted or stale annotation objects before modifying the PDF backend or undo history.
- Cancel deferred empty-text deletion when the widget is disposed; recheck contents and deletion state before executing.
- When the selected font lacks a character and Pango finds a single fallback covering the whole text, use that font for the PDF annotation. Preserve point size and keep the original font when it covers the text. This also repairs font selection when an existing annotation enters editing mode.
- Preserve the earlier Rust shell changes implementing Ctrl+S in-place saving.

No single fallback covering a mixed-script string means the existing font is retained. This does not solve every shaping, embedding, or cross-machine font issue.

## Source and build

`papers-50.2/` contains the full Papers source plus the patches. The upstream archive SHA-256 is `ae1bdcf1cd47cb50c9d84765784607f81c72df17dd6e6ad933fea14173d2b9f4` from `https://download.gnome.org/sources/papers/50/papers-50.2.tar.xz`. Patches are also available separately. GPL-2.0-or-later applies; retain upstream notices (see COPYING and source headers).

The published Flatpak preserves the existing 50.2 shell and PDF backend, replacing only `libppsview`. The root Meson project rebuilds that library against the installed Flatpak and GNOME SDK 50, using checked-in generated resources from the original build:

```sh
flatpak install flathub org.gnome.Sdk//50
flatpak run --devel --filesystem="$PWD" --command=sh org.gnome.Papers//inplace-save -c "cd '$PWD' && meson setup build && ninja -C build && meson test -C build --print-errorlogs"
```

`packaging/org.gnome.Papers.json` retains the original pinned dependency sources and adds the two patches. It can be used with `flatpak-builder --user --install --default-branch=inplace-save --force-clean staging packaging/org.gnome.Papers.json` for a full rebuild. **That full rebuild has not been executed for this release.** The shipped shell is byte-identical to the prior in-place-save installation. Runtime libraries are supplied by GNOME Platform 50.

Build the deb from the release Flatpak:

```sh
scripts/build-deb.sh /path/to/papers-fixed.flatpak
```

The installer pins the shipped Flatpak commit. A new export requires updating `expected_commit` in the launcher before rebuilding the deb.

## Validation

Eight lifecycle checks pass. Against old source, six stale/deferred-removal cases fail and two normal-operation cases pass; all eight pass after the fix. Chinese editing/render/save/reopen fails with the old font behavior and passes after the fix. A separate test completes three in-place JobSave/reopen cycles, checks annotation counts, and rejects stale deletion. See [incident and validation record](doc/incident-20260922.md).

Tests use disposable PDFs on ordinary local storage. Full GUI keystroke replay, every font/script, and OneDrive upload/download end-to-end have not been verified. No original user document is distributed.
