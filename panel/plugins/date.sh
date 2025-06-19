#!/usr/bin/env bash

date_default="󰃰 00/00/00"

date_start() {
    PLUGIN_ID=$1
    send bg "${YELLOW_BG}"
    send fg "${BLACK}"

    while :; do 
        current_date=$(date +"%a %b %d %Y")
        send update "󰃰 ${current_date}"
        sleep 1
    done
}
