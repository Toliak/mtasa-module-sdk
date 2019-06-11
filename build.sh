#!/usr/bin/env bash
mkdir -p cmake-build-release
if [[ "$1" = "TEST" ]]; then
    printf "Configure with test\n"
    cmake -H. -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON -G "CodeBlocks - Unix Makefiles"
else
    printf "Configure\n"
    cmake -H. -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - Unix Makefiles"
fi
printf "\nCompile\n"
cmake --build cmake-build-release
if [[ "$2" = "ON" ]]; then
    printf "\nInstall\n"
    mkdir -p tests/mtasa_server/x64/modules
    mv cmake-build-release/libModuleSdkTest.so tests/mtasa_server/x64/modules/libModuleSdkTest.so
fi