#!/bin/sh

if [[ -z $1 ]]; then
    echo Specify save folder
    exit 1
fi

for f in $(ls cmake-build-release | egrep "\.a|\.so"); do
    rm -fv $1/${f}
    cp -v cmake-build-release/${f} $1/${f}
done

exit 0