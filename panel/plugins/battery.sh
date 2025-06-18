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
}

battery_start() {
    PLUGIN_ID=$1
    send bg "${YELLOW_BG}"
    send fg "${BLACK}"
    while :; do 
        level=$(battery-level)
        icon=$(battery_icon $level)
        send update "${icon} ${level}%"

        sleep 5
    done
}


