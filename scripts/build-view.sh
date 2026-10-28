#!/usr/bin/env bash
# Rebuild libview against the installed Papers backend and GNOME SDK.
set -euo pipefail
root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
if [[ ! -f /.flatpak-info ]]; then
    exec flatpak run --devel --filesystem="$root" --command=bash \
        org.gnome.Papers//inplace-save "$root/scripts/build-view.sh" "$@"
fi
cd "$root"
meson setup --reconfigure build
ninja -C build
case "${1-build}" in
    build) ;;
    test)
        export GSETTINGS_BACKEND=memory GDK_BACKEND=broadway
        export BROADWAY_DISPLAY=:38 GTK_A11Y=none
        gtk4-broadwayd "$BROADWAY_DISPLAY" > "$root/build/broadway.log" 2>&1 &
        broadway_pid=$!
        trap 'kill "$broadway_pid" 2>/dev/null || true' EXIT
        dbus-run-session -- meson test -C build --print-errorlogs
        ;;
    *) echo 'Usage: scripts/build-view.sh [build | test]' >&2; exit 2 ;;
esac
