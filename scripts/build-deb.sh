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
Version: 50.2+fix20260922-1
Architecture: amd64
Maintainer: LumiaBlack51 <LumiaBlack51@users.noreply.github.com>
Depends: flatpak (>= 1.14)
Section: gnome
Priority: optional
Homepage: https://github.com/LumiaBlack51/papers-inplace-save
Description: Papers annotation fixes and in-place save (Flatpak installer)
 Bundles the fixed Papers Flatpak and a per-user installer/launcher.
 Preserves Ctrl+S in-place saving, fixes stale annotation removal and
 Chinese FreeText font fallback. Requires GNOME Platform 50; Flatpak
 may download that runtime on first installation. No root install hooks.
CONTROL
install -m 0755 "$root/packaging/papers-inplace-save" "$stage/usr/bin/"
install -m 0644 "$bundle" "$stage/usr/share/papers-inplace-save/papers-fixed.flatpak"
install -m 0644 "$root/packaging/papers-inplace-save.desktop" "$stage/usr/share/applications/"
install -m 0644 "$root/README.md" "$root/COPYING" "$stage/usr/share/doc/papers-inplace-save/"
install -m 0644 "$root/doc/incident-20260922.md" "$stage/usr/share/doc/papers-inplace-save/"
chmod 0755 "$stage"
mkdir -p "$root/dist"
dpkg-deb --build --root-owner-group "$stage" "$root/dist/papers-inplace-save_50.2+fix20260922-1_amd64.deb"
