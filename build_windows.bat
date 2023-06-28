@echo off

cmake -S . -B build -G "Visual Studio 17 2022" -A "x64" -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug

pause