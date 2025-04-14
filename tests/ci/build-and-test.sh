#!/bin/sh
set -e

if [ -d "build" ]; then
  rm build -rf
fi
set -x

export CCACHE_DIR=/ccache/

meson setup build \
    -Dlocal_checkout=true \
    -Dmaintainer=true \
    -Ddaemon_tests=true \
    $@

DUMMY_DESTDIR=/tmp/install-root/
rm -rf $DUMMY_DESTDIR

# Build & Install
ninja -C build
DESTDIR=$DUMMY_DESTDIR ninja -C build install

# Print ccache stats
ccache -s

# Run tests
mkdir -p /run/dbus/
dbus-daemon --system --print-address
meson test -C build \
    -v \
    --print-errorlogs
