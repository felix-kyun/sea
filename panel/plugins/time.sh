#!/usr/bin/env bash

time_default=" 00:00:00"

time_start() {
    PLUGIN_ID=$1
    send bg "${CYAN_BG}"
    send fg "${BLACK}"

    while :; do 
        current_time=$(date +"%H:%M:%S")
        send update " ${current_time}"
        sleep 1
    done
}
