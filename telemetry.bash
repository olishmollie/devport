#!/bin/bash
#
# telemetry.bash
#
# A script designed to log sio output. To log over a TCP socket,
# use with netcat:
#   $ nc -n <ip_address> <port> --exec='./telemetry.bash'
# And on the listening side:
#   $ nc -ln <port> | tee log.txt
#

while :
do
    sudo ./sio | \
        while IFS= read -r line
        do
            printf '%s %s\n' "$(date -Is)" "$line"
        done
    sleep 1
done
