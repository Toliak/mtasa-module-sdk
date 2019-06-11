#!/usr/bin/env bash
touch mtasa_server/mods/deathmatch/logs/server.log
screen -dmS mtasa mtasa_server/mta-server64

status=$(cat mtasa_server/mods/deathmatch/logs/server.log | grep "\[TEST TOTAL\]")
while [[ -z ${status} ]]; do
    status=$(cat mtasa_server/mods/deathmatch/logs/server.log | grep "\[TEST TOTAL\]")
done

status_ok=$(echo $status | grep "\[TEST TOTAL\]\[OK\]")
if [[ -z ${status_ok} ]]; then
    tail mtasa_server/mods/deathmatch/logs/server.log
    exit 1
else
    echo test passed
fi

screen -X -S mtasa quit

exit 0
