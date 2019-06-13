#!/bin/bash
mkdir -p cmake-build-release
if [[ $1 = "TEST" ]]; then
    printf "Configure with test\n"
    cmake -H. -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON
else
    printf "Configure\n"
    cmake -H. -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release
fi
printf "\nCompile\n"
cmake --build cmake-build-release
if [[ $2 = "ON" ]]; then
    printf "\nInstall\n"
    mkdir -p tests/mtasa_server/x64/modules
    mv -v cmake-build-release/libModuleSdkTest.so tests/mtasa_server/x64/modules/libModuleSdkTest.so
elif [[ ! -z $2 ]]; then
    printf "\nInstall (custom directory)\n"
    mkdir -p $2/x64/modules
    mv -v cmake-build-release/libModuleSdkTest.so $2/x64/modules/libModuleSdkTest.so
fi