#!/usr/bin/env bash

# source plugins
source scripts/battery-level.sh

# main loop
while :; do
    # get the battery level
    BATTERY_LEVEL=$(battery-level)

    # print the battery level
    echo -n "Battery Level: $BATTERY_LEVEL%"

    # sleep for 60 seconds before checking again
    sleep 60
    clear
done
