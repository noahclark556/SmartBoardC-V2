#!/bin/bash

# This will just add the noconsole flag to the build if set to 0.
DEV_BUILD=1

is_raspberry_pi() {
    if [[ $(uname -m) == "armv7l" || $(uname -m) == "aarch64" ]]; then
        return 0
    else
        return 1
    fi
}
is_mac() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        return 0
    else
        return 1
    fi
}

if is_raspberry_pi; then
    echo "Building voice daemon for Raspberry Pi"
    if [ $DEV_BUILD -eq 1 ]; then
        echo "Creating development build"
        # Development build
        python3 -m PyInstaller --onefile voice_daemon.py
    else
        echo "Creating production build"
        # Production build
        python3 -m PyInstaller --onefile --noconsole voice_daemon.py
    fi
elif is_mac; then
    echo "Building voice daemon for macOS"
    if [ $DEV_BUILD -eq 1 ]; then
        echo "Creating development build"
        # Development build
        pyinstaller --onefile voice_daemon.py
    else
        echo "Creating production build"
        # Production build
        pyinstaller --onefile --noconsole voice_daemon.py
    fi
else
    echo "Unsupported platform"
    exit 1
fi

echo "Copying build to daemon root"
cp dist/voice_daemon ./voice_daemon

echo "Cleaning up"
rm -rf dist
rm -rf build
rm -rf voice_daemon.spec

echo "Build complete"