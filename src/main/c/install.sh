#!/bin/bash

# release or debug build
BUILD_TYPE=$1 

# remove `build` dir (if it exists) and create a new one
rm -rf build && mkdir build && cd build

# ensure conan profile exists (needed for CI environments)
if ! conan profile show default &>/dev/null; then
    echo "Creating default conan profile..."
    conan profile detect --force
fi

# install antlr using conan package manager
conan install ../external/conanfile.py -of . --build=missing

# execute cmake to generate Makefile
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_MODULE_PATH=$(pwd) -DCMAKE_TOOLCHAIN_FILE=$(pwd)/conan_toolchain.cmake

# build native agent
make
