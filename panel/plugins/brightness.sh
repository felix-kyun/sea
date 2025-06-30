#!/usr/bin/env bash

brightness_default="󰃠 0%"
brightness_delay="${brightness_delay:-30}"

brightness_get() {
    brightnessctl -P get | sed 's/[^0-9]//g'
}

brightness_start() {
    while :; do
        send update "󰃠 $(brightness_get)%"
        sleep "${brightness_delay}"
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
