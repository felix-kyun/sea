#!/usr/bin/env bash

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
    log info "battery args $@"
    PLUGIN_ID=$1
    while :; do 
        level=$(battery-level)
        icon=$(battery_icon $level)
        send update "${icon} ${level}%"

        sleep 5
    done
}

battery_unload() {
    log info "Battery plugin unloaded"
    rm -f /tmp/battery.state
}

