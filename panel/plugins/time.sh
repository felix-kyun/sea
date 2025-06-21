#!/usr/bin/env bash

time_default=" 00:00:00"

time_start() {
    PLUGIN_ID=$1

    while :; do 
        current_time=$(date +"%H:%M")
        send update " ${current_time}"
        sleep 60
    done
}
