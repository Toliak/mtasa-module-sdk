#!/bin/sh
for f in $(ls cmake-build-release | egrep "\.a|\.so"); do
    rm -fv bin/${f}
    cp -v cmake-build-release/${f} bin/${f}
done