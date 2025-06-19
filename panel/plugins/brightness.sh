#!/usr/bin/env bash

brightness_onload() {
    log info "Brightness plugin loaded"
}

brightness_start() {
    PLUGIN_ID=$1
    send bg "${GREEN_BG}"
    send fg "${BLACK}"
    while :; do 
        send update "󰃠 $(brightnessctl -P get | sed 's/[^0-9]//g')%"
        sleep 1
    done
}

