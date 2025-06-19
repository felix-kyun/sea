#!/usr/bin/env bash

ram_default="󰍛 0 MiB"

ram_start() {
    PLUGIN_ID=$1
    send bg "${MAGENTA_BG}"
    send fg "${BLACK}"

    while :; do 
        ram=$(free --mebi | awk '/^Mem:/ {print $3}')
        send update "󰍛 ${ram} MiB"
        sleep 1
    done
}
