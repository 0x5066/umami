#!/bin/bash

# Check if a parameter was passed
if [ -z "$1" ]; then
    echo "Usage: $0 [linux|mingw]"
    exit 1
fi

# Clean the build directory
rm -r build

# Function to check if the build fails
check_build_status() {
    if [ $? -ne 0 ]; then
        exit 1
    fi
}

if [ "$1" == "linux" ]; then
    cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
    check_build_status

    cmake --build build
    check_build_status

    cmake --build build
    check_build_status

    cp -r freeform/ build/freeform/ -v
    cp -r skins/ build/skins/ -v

    build/umami "$2" "$3" #&> log.txt &
elif [ "$1" == "mingw" ]; then
    export WINEPREFIX=$HOME/.wineumami
    i686-w64-mingw32-cmake -B build -G Ninja
    check_build_status

    cmake --build build
    check_build_status

    cmake --build build
    check_build_status

    cp -r skins/ build/skins/ -v

    wine explorer /desktop=shell build/umami.exe
else
    echo "Invalid option. Use 'linux' or 'mingw'."
    exit 1
fi
