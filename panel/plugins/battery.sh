#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source $CURRENT_DIR/../config.sh

battery-level() {
        upower -e | grep -m 1 battery | xargs -I {} upower -i {} | awk '/percentage/ { print $2 }'
}

battery_onload() {
    log info "Battery plugin loaded"
}

battery_start() {
    PLUGIN_ID=$1
    while :; do 
        sleep 5
        send "$(battery-level)"
    done
}
