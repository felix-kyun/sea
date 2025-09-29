#!/usr/bin/env bash

battery_default=${battery_default:-"󰁹 100%"}
battery_delay="${battery_delay:-30}"

battery-level() {
    upower -e | grep -m 1 battery | xargs -I {} upower -i {} | awk '/percentage/ { print $2 }' | tr -d '%'
}

battery_icon() {
    local level=$1
    if [ "$level" -ge 80 ]; then
        echo "󰁹"
    elif [ "$level" -ge 50 ]; then
        echo "󰂃"
    elif [ "$level" -ge 20 ]; then
        echo "󰂄"
    else
        echo "󰁺"
    fi
}

battery_onload() {
    log info "Battery plugin loaded"
    touch /tmp/battery.state
}

battery_start() {
    while :; do
        level=$(battery-level)
        icon=$(battery_icon $level)
        send update "${icon} ${level}%"

        sleep "${battery_delay}"
    done
}

battery_unload() {
    log info "Battery plugin unloaded"
    rm -f /tmp/battery.state
}
