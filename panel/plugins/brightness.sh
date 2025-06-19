#!/usr/bin/env bash

brightness_start() {
    PLUGIN_ID=$1
    while :; do 
        send update "󰃠 $(brightnessctl -P get | sed 's/[^0-9]//g')%"
        sleep 1
    done
}

