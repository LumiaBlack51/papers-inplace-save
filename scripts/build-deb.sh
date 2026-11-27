#!/usr/bin/env bash
set -euo pipefail
root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
bundle=${1:?Usage: build-deb.sh /path/to/papers-fixed.flatpak}
bundle=$(realpath -- "$bundle")
[ -f "$bundle" ]
stage=$(mktemp -d -t papers-deb.XXXXXX)
trap 'rm -rf -- "$stage"' EXIT
install -d "$stage/DEBIAN" "$stage/usr/bin" "$stage/usr/share/papers-inplace-save" "$stage/usr/share/applications" "$stage/usr/share/doc/papers-inplace-save"
cat > "$stage/DEBIAN/control" <<CONTROL
Package: papers-inplace-save
Version: 51.1+fix20261127-1
Architecture: amd64
Maintainer: LumiaBlack51 <LumiaBlack51@users.noreply.github.com>
Depends: flatpak (>= 1.14)
Section: gnome
Priority: optional
Homepage: https://github.com/LumiaBlack51/papers-inplace-save
Description: Papers annotation fixes and in-place save (Flatpak installer)
 Bundles the fixed Papers Flatpak and a per-user installer/launcher.
 Ctrl+S saves in place and a close request waits for all pending saves.
 Includes annotation, font, and selection fixes. Requires GNOME Platform 51.
 Flatpak may download the runtime on first installation.
 No root install hooks.
CONTROL
install -m 0755 "$root/packaging/papers-inplace-save" "$stage/usr/bin/"
install -m 0644 "$bundle" "$stage/usr/share/papers-inplace-save/papers-fixed.flatpak"
install -m 0644 "$root/packaging/papers-inplace-save.desktop" "$stage/usr/share/applications/"
install -m 0644 "$root/README.md" "$root/COPYING" "$stage/usr/share/doc/papers-inplace-save/"
install -m 0644 "$root/doc/incident-20260922.md" "$stage/usr/share/doc/papers-inplace-save/"
install -m 0644 "$root/doc/save-refresh-20261007.md" "$stage/usr/share/doc/papers-inplace-save/"
install -m 0644 "$root/doc/popup-state-20261029.md" "$stage/usr/share/doc/papers-inplace-save/"
chmod 0755 "$stage"
mkdir -p "$root/dist"
dpkg-deb --build --root-owner-group "$stage" "$root/dist/papers-inplace-save_51.1+fix20261127-1_amd64.deb"
