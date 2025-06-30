#!/usr/bin/env bash

time_default=" 00:00:00"
time_delay="${time_delay:-60}"

time_start() {
    while :; do
        current_time=$(date +"%H:%M")
        send update " ${current_time}"
        sleep "${time_delay}"
    done
}
