# Papers in-place save

Downstream GNOME Papers 51.1 for Linux, using GNOME Platform 51. Ctrl+S saves to the current PDF while keeping the reading position. Ctrl+Shift+S opens Save As.

If you close a window while Papers is saving, it stays open until the active write and any queued Ctrl+S finish. A failed write leaves the document open with **Retry** and **Save As**. Once a write to a mounted sync folder completes, its sync client can upload independently.

This build also carries forward readable text selection, stale annotation deletion safeguards, Chinese FreeText font fallback when one font covers the text, and persistence of manually closed note popups.

## Install

Download the Flatpak and `SHA256SUMS` from [Releases](https://github.com/LumiaBlack51/papers-inplace-save/releases).

```sh
sha256sum --ignore-missing -c SHA256SUMS
flatpak install --user --or-update ./papers-fixed.flatpak
flatpak run --user org.gnome.Papers//inplace-save
```

The amd64 deb bundles the same Flatpak and a per-user launcher. After installing the deb, run `papers-inplace-save --install` as your desktop user. The previous user-level branch upgrades in place and retains application data. An already open window continues using the version that launched it.

The last 50.2 build is available in the [50.2 archive](https://github.com/LumiaBlack51/papers-inplace-save/releases/tag/v50.2-fix20261121-1). Earlier validation records are under [doc](doc/).

## Source and build

`papers-51.1/` contains the patched release sources. The Flatpak manifest verifies the [GNOME 51.1 archive](https://download.gnome.org/sources/papers/51/papers-51.1.tar.xz), then applies the two files under `patches/`. The project and Papers source are licensed under GPL-2.0-or-later. See [COPYING](COPYING).

```sh
flatpak install --user flathub org.gnome.Sdk//51 org.freedesktop.Sdk.Extension.rust-stable//26.08
scripts/export-flatpak.sh
```

The build host needs Flatpak Builder, elfutils, and AppStream Compose. The export script builds Papers and its pinned dependencies, then writes `dist/papers-fixed.flatpak`. For a new release, set `expected_commit` in the launcher to the exported application commit and update the deb version in `scripts/build-deb.sh`.

```sh
scripts/build-deb.sh dist/papers-fixed.flatpak
python3 scripts/test-installer.py
```

The Meson suite includes the shell's save and file-monitor regressions and the selection, popup, annotation lifecycle, FreeText, and save/reopen tests. Run the GTK tests on a private display and D-Bus session with in-memory settings. The fixtures are disposable or from the upstream source distribution.
