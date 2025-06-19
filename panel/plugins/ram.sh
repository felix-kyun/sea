#!/usr/bin/env bash

ram_default="  0%"

ram_start() {
    PLUGIN_ID=$1
    send bg "${MAGENTA_BG}"
    send fg "${BLACK}"

    while :; do 
        ram=$(free --mebi | awk '/^Mem:/ {print $3/$2 * 100.0}' | awk '{printf "%.0f", $1}')
        send update "  ${ram}%"
        sleep 1
    done
}
