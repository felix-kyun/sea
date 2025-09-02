#!/usr/bin/env bash

icon_wifi_on="󰖩"
icon_wifi_off="󰖪"

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source "$CURRENT_DIR/../utils/fzf_theme_override.sh"

connected() {
    nmcli -f WIFI g | grep -q "enabled"
}

wifi() {
    nmcli radio wifi "$1"
}

blue() {
    echo -e "$(tput setaf 4)$1$(tput sgr0)"
}

prompt() {
    if connected; then
        echo "off:$icon_wifi_off Turn Off Wi-Fi"
    else
        echo "on:$icon_wifi_on Turn On Wi-Fi"
    fi

    list=$(nmcli -f "IN-USE,SSID" device wifi list | sed '1d')
    connected_devices=$(echo "$list" | grep '\*' | awk '{print $2}')
    available_devices=$(echo "$list" | grep -v '\*' | awk '{print $1}')

    for device in $connected_devices; do
        echo "disconnect;${device}:$(blue "󰤨 ${device} (Connected)")"
    done

    for device in $available_devices; do
        echo "connect;${device}:󰤨 ${device}"
    done

    echo "exit:󰅖 Exit"
    sleep 1
}

main() {
    action="$(prompt \
        | fzf \
            --prompt 'Wi-Fi: ' \
            --reverse \
            --no-multi \
            --ansi \
            --border none \
            --no-scrollbar \
            --delimiter ':' \
            --layout reverse \
            --with-nth '{2}' \
            --accept-nth 1) \
            )"

    case "${action}" in
        on*) wifi on;;
        off*) wifi off;;
        disconnect*) nmcli con down id "${action#*;}" ;;
    esac
}
