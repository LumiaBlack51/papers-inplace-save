#!/usr/bin/env bash
# Retain the tested installed libraries and replace only the rebuilt Rust shell.
set -euo pipefail
root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
binary="$root/build-shell/target/release/papers"
[[ -x "$binary" ]] || { echo 'Run scripts/build-shell.sh build first.' >&2; exit 1; }
deployment=$(flatpak info --user --show-location org.gnome.Papers//inplace-save)
stage=$(mktemp -d -t papers-shell-export.XXXXXX)
trap 'rm -rf -- "$stage"' EXIT
cp -a --reflink=auto "$deployment/files" "$stage/files"
cp "$deployment/metadata" "$stage/metadata"
install -m 0755 "$binary" "$stage/files/bin/papers"
strip "$stage/files/bin/papers"
flatpak build-finish --command=papers "$stage"
mkdir -p "$root/dist"
flatpak build-export \
    --subject='Papers 50.2: Keep the reading view during in-place save' \
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
sha256sum papers-fixed.flatpak papers-inplace-save_50.2+fix20261007-1_amd64.deb > SHA256SUMS
