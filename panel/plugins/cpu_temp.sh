#!/usr/bin/env bash

cpu_temp_default=" N/A"

cpu_temp_start() {
    PLUGIN_ID=$1

    while :; do 
        cpu_temp=$(awk '{print $1/1000 "°C"}' /sys/class/thermal/thermal_zone*/temp)
        if [[ -z "$cpu_temp" ]]; then
            cpu_temp="N/A"
        fi
        send update " ${cpu_temp}"
        sleep 10
    done
}
