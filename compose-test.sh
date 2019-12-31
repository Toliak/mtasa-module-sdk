#! /bin/sh

export USER_ID=$(id -u)
export GROUP_ID=$(id -g)

mkdir -p cmake-build-modules
docker-compose up --build 2>&1 | tee compose-log.txt

if grep -E -q "exited with code [1-9]" compose-log.txt; then
    exit 1
fi
if grep -E -q "non-zero code: [1-9]" compose-log.txt; then
    exit 1
fi

rm compose-log.txt
exit 0