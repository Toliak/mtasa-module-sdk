#!/bin/bash

mkdir -pv cmake-build-release
if [[ $1 = "test" ]]; then
    printf "Configure with test\n"
    cmake -H. -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON
else
    printf "Configure\n"
    cmake -H. -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release
fi

printf "\nCompile\n"
cmake --build cmake-build-release