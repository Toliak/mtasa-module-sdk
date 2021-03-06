#!/bin/bash

function globalClear() {
    pkill mta-server64
}

function exitFail() {
    globalClear
    exit 1
}

function exitSuccess() {
    globalClear
    exit 0
}

SOURCE_DIR=$( cd $( dirname ${BASH_SOURCE[0]} ) >/dev/null 2>&1 && pwd )
SERVER_FOLDER=${SOURCE_DIR}
if [[ ! -z $1 ]]; then
    SERVER_FOLDER=$1
fi

mkdir -p ${SERVER_FOLDER}/mods/deathmatch/logs
rm -fv ${SERVER_FOLDER}/mods/deathmatch/logs/server.log
touch ${SERVER_FOLDER}/mods/deathmatch/logs/server.log
${SERVER_FOLDER}/mta-server64 >> /dev/null &

sleep 0.5s
status=$(cat ${SERVER_FOLDER}/mods/deathmatch/logs/server.log | grep "\[TEST TOTAL\]")
while [[ -z ${status} ]]; do
    sleep 0.5s
    if [[ -z $(ps -A | grep mta-server64) ]]; then
        echo MTA-SA server proccess not found
        exitFail
    fi
    if [[ ! -z $(cat ${SERVER_FOLDER}/mods/deathmatch/logs/server.log | grep "ERROR: ") ]]; then
        echo MTA-SA script error
        cat ${SERVER_FOLDER}/mods/deathmatch/logs/server.log
        exitFail
    fi
    status=$(cat ${SERVER_FOLDER}/mods/deathmatch/logs/server.log | grep "\[TEST TOTAL\]")
done

cat ${SERVER_FOLDER}/mods/deathmatch/logs/server.log
printf "\n\n\n"
status_ok=$(echo ${status} | grep "\[TEST TOTAL\]\[OK\]")

if [[ -z ${status_ok} ]]; then
    echo Tests failed
    exitFail
else
    echo Tests passed
    exitSuccess
fi
