#!/usr/bin/env bash
set -euo pipefail
root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
builder=${FLATPAK_BUILDER:-flatpak-builder}
work=${PAPERS_BUILD_DIR:-"$root/build"}
mkdir -p "$work" "$root/dist"

nice -n 10 "$builder" --user --disable-rofiles-fuse --keep-build-dirs \
    --jobs=4 --state-dir="$work/state" --repo="$work/repo" \
    --default-branch=inplace-save --force-clean \
    --subject='Papers 51.1: Wait for the final write before closing' \
    "$work/staging" "$root/packaging/org.gnome.Papers.json"

flatpak build-bundle "$work/repo" "$root/dist/papers-fixed.flatpak" \
    org.gnome.Papers inplace-save
