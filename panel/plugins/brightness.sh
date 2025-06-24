#!/usr/bin/env bash

brightness_default="󰃠 0%"
brightness_delay=60

brightness_get() {
    brightnessctl -P get | sed 's/[^0-9]//g'
}

brightness_start() {
    PLUGIN_ID=$1
    while :; do 
        send update "󰃠 $(brightness_get)%"
        sleep "${brightness_delay}"
    done
}

brightness_on_scrollup() {
    PLUGIN_ID=$1
    brightnessctl set +5%
    send update "󰃠 $(brightness_get)%"

}

brightness_on_scrolldown() {
    PLUGIN_ID=$1
    brightnessctl set 5%-
    send update "󰃠 $(brightness_get)%"
}
