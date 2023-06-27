#!/bin/bash

if test \( $# -ne 1 \);
then
    echo "Usage: build.sh arch config"
    echo ""
    echo "Configs:"
    echo "  debug   -   build with the debug configuration"
    echo "  release -   build with the release configuration"
    echo ""
    exit 1
fi


if test \( \( -n "$1" \) -a \( "$1" = "debug" \) \);then
    CONFIG=" Debug"
elif test \( \( -n "$1" \) -a \( "$1" = "release" \) \);then
    CONFIG=" Release"
else
    echo "The config \"$1\" is not supported!"
    echo ""
    echo "Configs:"
    echo "  debug   -   build with the debug configuration"
    echo "  release -   build with the release configuration"
    echo ""
    exit 1
fi

cmake -S . -B build -G "Xcode" -DCMAKE_TOOLCHAIN_FILE="~/Program/Library/vcpkg/scripts/buildsystems/vcpkg.cmake"
# cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="~/Program/Library/vcpkg/scripts/buildsystems/vcpkg.cmake"
# cmake -S . -B build

cmake --build build --config "${CONFIG}"
