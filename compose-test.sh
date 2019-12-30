#! /bin/sh

export USER_ID=$(id -u)
export GROUP_ID=$(id -g)

mkdir -p cmake-build-modules
docker-compose up --build 2>&1 | tee compose-log.txt
if [[ ! -z $(cat compose-log.txt | egrep "exited with code [1-9]") ]]; then
    exit 1
fi

rm compose-log.txt
exit 0