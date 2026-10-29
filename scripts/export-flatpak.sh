#!/usr/bin/env bash
# Retain the installed dependencies and replace the rebuilt shell and libview.
set -euo pipefail
root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
deployment=$(flatpak info --user --show-location org.gnome.Papers//inplace-save)
case "${1-}" in
    '') binary="$root/build-shell/target/release/papers" ;;
    --reuse-installed-shell) binary="$deployment/files/bin/papers" ;;
    *) echo 'Usage: scripts/export-flatpak.sh [--reuse-installed-shell]' >&2; exit 2 ;;
esac
library="$root/build/libppsview-4.0.so.5.0.0"
[[ -x "$binary" ]] || { echo 'Run scripts/build-shell.sh build first.' >&2; exit 1; }
[[ -f "$library" ]] || { echo 'Run scripts/build-view.sh build first.' >&2; exit 1; }
stage=$(mktemp -d -t papers-export.XXXXXX)
trap 'rm -rf -- "$stage"' EXIT
cp -a --reflink=auto "$deployment/files" "$stage/files"
cp "$deployment/metadata" "$stage/metadata"
install -m 0755 "$binary" "$stage/files/bin/papers"
install -m 0755 "$library" "$stage/files/lib/libppsview-4.0.so.5.0.0"
strip "$stage/files/bin/papers" "$stage/files/lib/libppsview-4.0.so.5.0.0"
# These debug files describe the previous executable/library.
rm -f "$stage/files/lib/debug/bin/papers.debug" \
    "$stage/files/lib/debug/lib/libppsview-4.0.so.5.0.0.debug"
flatpak build-finish --command=papers "$stage"
mkdir -p "$root/dist" "$root/build-shell"
flatpak build-export \
    --subject='Papers 50.2: Preserve closed note popups and existing save fixes' \
    "$root/build-shell/repo" "$stage" inplace-save | tee "$root/build-shell/export.log"
commit=$(sed -n 's/^Commit: //p' "$root/build-shell/export.log")
[[ "$commit" =~ ^[0-9a-f]{64}$ ]] || { echo 'Missing exported commit.' >&2; exit 1; }
flatpak build-bundle "$root/build-shell/repo" "$root/dist/papers-fixed.flatpak" \
    org.gnome.Papers inplace-save
python3 - "$root/packaging/papers-inplace-save" "$commit" <<'PY'
from pathlib import Path
import re
import sys
launcher = Path(sys.argv[1])
launcher.write_text(re.sub(r"^expected_commit=[0-9a-f]{64}$",
                          "expected_commit=" + sys.argv[2], launcher.read_text(), flags=re.M))
PY
"$root/scripts/build-deb.sh" "$root/dist/papers-fixed.flatpak"
cd "$root/dist"
sha256sum papers-fixed.flatpak papers-inplace-save_50.2+fix20261029-1_amd64.deb > SHA256SUMS
