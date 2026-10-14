#!/bin/sh
preset="${RTYPE_PRESET:-linux-debug}"
bin="$(dirname "$0")/build/$preset/bin/r-type_client"
[ -x "$bin" ] || { echo "Not built yet. Run: cmake --workflow --preset $preset" >&2; exit 1; }
exec "$bin" "$@"
