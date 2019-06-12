#!/bin/bash
SOURCE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

mkdir -p ${SOURCE_DIR}/mtasa_server/mods/deathmatch/logs
touch ${SOURCE_DIR}/mtasa_server/mods/deathmatch/logs/server.log
screen -dmS mtasa ${SOURCE_DIR}/mtasa_server/mta-server64

status=$(cat ${SOURCE_DIR}/mtasa_server/mods/deathmatch/logs/server.log | grep "\[TEST TOTAL\]")
while [[ -z ${status} ]]; do
    sleep 1s
    if [[ -z $(ps -A | grep mta-server64) ]]; then
        echo MTA-SA server proccess not found
        exit 1
    fi
    if [[ ! -z $(cat ${SOURCE_DIR}/mtasa_server/mods/deathmatch/logs/server.log | grep "ERROR: ") ]]; then
        echo MTA-SA script error
        tail ${SOURCE_DIR}/mtasa_server/mods/deathmatch/logs/server.log
        exit 1
    fi
    status=$(cat ${SOURCE_DIR}/mtasa_server/mods/deathmatch/logs/server.log | grep "\[TEST TOTAL\]")
done

status_ok=$(echo $status | grep "\[TEST TOTAL\]\[OK\]")
if [[ -z ${status_ok} ]]; then
    echo Test failed
    tail -n 25 ${SOURCE_DIR}/mtasa_server/mods/deathmatch/logs/server.log
    exit 1
else
    echo Test passed
fi

screen -X -S mtasa quit

exit 0
