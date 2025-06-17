#!/usr/bin/env bash

battery-level() {
    local BATTERY=$(upower -e | grep battery)
    local BATTERY_PERCENTAGE=$(upower -i "$BATTERY" | grep "percentage" | awk '{print $2}' | tr -d '%')

    echo $BATTERY_PERCENTAGE
}

battery_onload() {
    echo "starting battery plugin"
}
