#!/usr/bin/env bash

bluetooth_default="󰂯 Disconnected"
bluetooth_len=0

bluetooth_onload() {
    PLUGIN_ID=$1
    local initial_device=$(bluetoothctl devices Connected | awk 'NR==1 { for(i=3; i<=NF; i++) printf "%s ", $i; print "\b" }')
    if [[ -n "$initial_device" ]]; then
        local device_name=$(bluetooth_get_device_name "$initial_device")
        send update "󰂯 $device_name"
    else
        send update "$bluetooth_default"
    fi
}

bluetooth_start() {
    PLUGIN_ID=$1
    local current_dir=$(dirname "${BASH_SOURCE[0]}")

    bluetoothctl --monitor | awk -f "${current_dir}/utils/bluetooth.awk" | while read -r data; do
        IFS='|' read -r status name <<< "${data}"
        if [[ "$status" == "connected" ]]; then
            send update "󰂯 $(bluetooth_get_device_name "$name")"
        else
            send update "$bluetooth_default"
        fi
    done

}

bluetooth_get_device_name() {
    echo "$(bluetoothctl info $1 | awk '/^.*Name:/ { for(i=2; i<=NF; i++) printf "%s ", $i; print "" }')"
}
