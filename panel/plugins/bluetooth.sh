#!/usr/bin/env bash

bluetooth_pipe=${bluetooth_pipe:-"/tmp/bluetooth.pipe"}
bluetooth_default="󰂯 Disconnected"

bluetooth_onload() {
    local initial_device=$(bluetooth_get_first_connected_device)
    send update "$initial_device"
}

bluetooth_start() {
    local current_dir=$(dirname "${BASH_SOURCE[0]}")

    # create named pipe for bluetoothctl output
    # keep it open or else bluetoothctl will exit
    [[ -p "$bluetooth_pipe" ]] || mkfifo /tmp/bluetooth.pipe
    exec 99<>"$bluetooth_pipe"

    bluetoothctl --monitor <"$bluetooth_pipe" |
        stdbuf -oL awk -f "${current_dir}/utils/bluetooth.awk" |
        while read -r data; do
            IFS='|' read -r status name <<<"${data}"
            if [[ "$status" == "connected" ]]; then
                send update "󰂯 $(bluetooth_get_device_name "$name")"
            else
                local top_device=$(bluetooth_get_first_connected_device)
                send update "$top_device"
            fi
        done

    exec 99<&-
}

bluetooth_get_device_name() {
    echo "$(bluetoothctl info $1 | awk '/^.*Name:/ { for(i=2; i<=NF; i++) printf "%s ", $i; print "" }')"
}

bluetooth_get_first_connected_device() {
    local device=$(bluetoothctl devices Connected | awk 'NR==1 { for(i=3; i<=NF; i++) printf "%s ", $i; print "\b" }')
    if [[ -n "$device" ]]; then
        echo "󰂯 ${device}"
    else
        echo "${bluetooth_default}"
    fi
}
