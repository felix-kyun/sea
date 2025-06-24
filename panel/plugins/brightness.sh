#!/usr/bin/env bash

brightness_default="󰃠 0%"
brightness_get() {
    brightnessctl -P get | sed 's/[^0-9]//g'
}

brightness_start() {
    PLUGIN_ID=$1
    while :; do 
        send update "󰃠 $(brightness_get)%"
        sleep 60
    done
}

brightness_on_scrollup() {
    brightnessctl set +5%
    send update "󰃠 $(brightness_get)%"

}

brightness_on_scrolldown() {
    brightnessctl set 5%-
    send update "󰃠 $(brightness_get)%"
}
