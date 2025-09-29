#!/usr/bin/env bash

cpu_temp_default=" N/A"
cpu_temp_delay="${cpu_temp_delay:-10}"

cpu_temp_start() {
    while :; do
        cpu_temp=$(awk '{print $1/1000 "°C"}' /sys/class/thermal/thermal_zone*/temp)
        if [[ -z "$cpu_temp" ]]; then
            cpu_temp="N/A"
        fi
        send update " ${cpu_temp}"
        sleep "${cpu_temp_delay}"
    done
}
