#!/usr/bin/env bash

# time in seconds to wait for scan to complete
SCAN_TIMEOUT=10
# time in seconds to wait between menu transitions
MENU_INTERVAL=0.5

# controller function
power() {
    bluetoothctl show | grep -q "Powered: yes"
}

scan() {
    bluetoothctl show | grep -q "Discovering: yes"
}

pairable() {
    bluetoothctl show | grep -q "Pairable: yes"
}

discoverable() {
    bluetoothctl show | grep -q "Discoverable: yes"
}

toggle_power() {
    if power; then
        bluetoothctl power off
    else
        bluetoothctl power on
    fi
}

toggle_scan() {
    if scan; then
        bluetoothctl scan off
    else
        bluetoothctl --timeout "$SCAN_TIMEOUT" scan on &>/dev/null & disown
        scan_pid=$!
    fi
}

toggle_pairable() {
    if pairable; then
        bluetoothctl pairable off
    else
        bluetoothctl pairable on
    fi
}

toggle_discoverable() {
    if discoverable; then
        bluetoothctl discoverable off
    else
        bluetoothctl discoverable on
    fi
}

# device functions

connected() {
    bluetoothctl info "$1" | grep -q "Connected: yes"
}

paired() {
    bluetoothctl info "$1" | grep -q "Paired: yes"
}

trusted() {
    bluetoothctl info "$1" | grep -q "Trusted: yes"
}

toggle_connected() {
    if connected "$1"; then
        bluetoothctl disconnect "$1"
    else
        bluetoothctl connect "$1"
    fi
}

toggle_paired() {
    if paired "$1"; then
        bluetoothctl remove "$1"
    else
        bluetoothctl pair "$1"
    fi
}

toggle_trusted() {
    if trusted "$1"; then
        bluetoothctl untrust "$1"
    else
        bluetoothctl trust "$1"
    fi
}

# menu
device_list() {
    local device=$(bluetoothctl devices \
        | awk '{
            printf "%s|", $2;
            for (i=3; i<=NF; i++) {
                printf "%s ", $i;
            }
            printf "\n";
        }' \
        | fzf \
            --delimiter '|' \
            --reverse \
            --border none \
            --no-scrollbar \
            --no-multi \
            --with-nth '{2}')

    if [[ -n "$device" ]]; then
        device_menu "$device"
    fi
}

device_menu() {
    IFS='|' read -r device name <<< "$1"
    local actions=(
        "Connected: $(state connected "$device")|toggle_connected"
        "Paired: $(state paired "$device")|toggle_paired"
        "Trusted: $(state trusted "$device")|toggle_trusted"
    )

    local selected=$(printf '%s\n' "${actions[@]}" \
        | fzf \
            --header "Device: $name" \
            --reverse \
            --no-multi \
            --border none \
            --no-scrollbar \
            --delimiter '|' \
            --layout reverse \
            --with-nth '{1}')

    if [[ -n "$selected" ]]; then
        case "$selected" in
            toggle_connected*) toggle_connected "$device" ;;
            toggle_paired*) toggle_paired "$device" ;;
            toggle_trusted*) toggle_trusted "$device" ;;
        esac
        sleep "$MENU_INTERVAL"
        device_menu "$1"
    fi

}

main_menu() {
    local actions=(
        "Power: $(state power)|toggle_power"
        "Scan: $(state scan)|toggle_scan"
        "Pairable: $(state pairable)|toggle_pairable"
        "Discoverable: $(state discoverable)|toggle_discoverable"
        "Devices|devices"
    )

    local selected=$(printf '%s\n' "${actions[@]}" \
        | fzf \
            --reverse \
            --no-multi \
            --border none \
            --no-scrollbar \
            --delimiter '|' \
            --layout reverse \
            --with-nth '{1}' \
            --accept-nth 2)

    if [[ -n "$selected" ]]; then
        case "$selected" in
            devices) 
                device_list
                ;;
            *)
                $selected
                sleep 0.5
                main_menu
                ;;
        esac
    fi
}

# utils
ternery() {
    $1 && echo $2 || echo $3
}

state() {
    ternery "$1" on off
}

trap cleanup EXIT SIGINT SIGTERM

cleanup() {
    if [[ -n "${scan_pid}" ]]; then
        kill "$scan_pid" &>/dev/null
    fi
}
