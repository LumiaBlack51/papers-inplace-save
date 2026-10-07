#!/usr/bin/env bash
# Build the Rust shell against the installed, fixed Papers Flatpak libraries.
set -euo pipefail
root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
if [[ ! -f /.flatpak-info ]]; then
    exec flatpak run --devel --share=network --filesystem="$root" --command=bash \
        org.gnome.Papers//inplace-save "$root/scripts/build-shell.sh" "$@"
fi
export PATH="/usr/lib/sdk/rust-stable/bin:$PATH"
export CARGO_HOME="$root/build-shell/cargo-home"
export CARGO_TARGET_DIR="$root/build-shell/target"
export CODEGEN_BUILD_DIR="$root/build-shell"
export PAPERS_RESOURCES_FILE="$root/build-shell/pps-resources.gresource"
export SYSTEM_DEPS_PAPERS_VIEW_4_0_NO_PKG_CONFIG=1
export SYSTEM_DEPS_PAPERS_VIEW_4_0_SEARCH_NATIVE=/app/lib
export SYSTEM_DEPS_PAPERS_VIEW_4_0_LIB=ppsview-4.0
export SYSTEM_DEPS_PAPERS_DOCUMENT_4_0_NO_PKG_CONFIG=1
export SYSTEM_DEPS_PAPERS_DOCUMENT_4_0_SEARCH_NATIVE=/app/lib
export SYSTEM_DEPS_PAPERS_DOCUMENT_4_0_LIB=ppsdocument-4.0
mkdir -p "$CODEGEN_BUILD_DIR/resources"
cat > "$CODEGEN_BUILD_DIR/config.rs" <<'CONFIG'
pub const GETTEXT_PACKAGE: &str = "papers";
pub const APP_ID: &str = "org.gnome.Papers";
pub const VERSION: &str = "50.2";
pub const OBJECT_PROFILE: &str = "";
pub const PROFILE: &str = "";
pub const PPS_LOCALEDIR: &str = "/app/share/locale";
CONFIG
source_resources="$root/papers-50.2/shell/resources"
blueprint-compiler batch-compile "$CODEGEN_BUILD_DIR/resources" \
    "$source_resources" "$source_resources"/*.blp
sed 's/@APP_ID@/org.gnome.Papers/g' "$source_resources/papers.gresource.xml.in" \
    > "$CODEGEN_BUILD_DIR/resources/papers.gresource.xml"
cp /app/share/metainfo/org.gnome.Papers.metainfo.xml "$CODEGEN_BUILD_DIR/resources/"
glib-compile-resources "$CODEGEN_BUILD_DIR/resources/papers.gresource.xml" \
    --sourcedir="$CODEGEN_BUILD_DIR/resources" --sourcedir="$source_resources" \
    --sourcedir="$root/papers-50.2/data" --target="$PAPERS_RESOURCES_FILE"
cd "$root/papers-50.2"
case "${1-build}" in
    build) cargo build --release --locked -p papers --features with-keyring ;;
    test)
        export GSETTINGS_BACKEND=memory
        export GDK_BACKEND=broadway
        export BROADWAY_DISPLAY=:37
        export GTK_A11Y=none
        gtk4-broadwayd "$BROADWAY_DISPLAY" > "$root/build-shell/broadway.log" 2>&1 &
        broadway_pid=$!
        trap 'kill "$broadway_pid" 2>/dev/null || true' EXIT
        dbus-run-session -- cargo test --locked -p papers --features with-keyring -- --test-threads=1
        ;;
    *) echo 'Usage: scripts/build-shell.sh [build | test]' >&2; exit 2 ;;
esac
