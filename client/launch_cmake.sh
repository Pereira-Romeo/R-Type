#!/bin/sh
# Build the whole project from the root (client is a subdirectory now)
cd "$(dirname "$0")/.." || exit 1
case "$(uname)" in Darwin) os=macos ;; *) os=linux ;; esac
preset="${RTYPE_PRESET:-$os-debug}"
[ -f vcpkg/bootstrap-vcpkg.sh ] || git submodule update --init vcpkg
cmake --workflow --preset "$preset"
