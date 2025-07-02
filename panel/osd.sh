#!/usr/bin/env bash

# helper script to send notifications to the panel
CURRENT_DIR="$(dirname "${BASH_SOURCE[0]}")"

source "${CURRENT_DIR}/utils.sh"
source "${CURRENT_DIR}/config.sh"

osd_send() {
    echo "osd:set:${1}" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
}

send_volume() {
    echo "volume:update:$1" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
}

send_brightness() {
    echo "brightness:update:󰃠 ${1}%" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
}

get_volume() {
    pactl get-sink-volume @DEFAULT_SINK@ | grep -Eo " [0-9]{2}% " | tr -d " %" | head -n1
}

set_volume() {
    pactl set-sink-volume @DEFAULT_SINK@ "$1" &>/dev/null
}

get_b() {
    brightnessctl -P get 2>/dev/null
}

change_b() {
    brightnessctl set "$1" &>/dev/null
}

if [[ "$1" == vol* ]]; then
    case "$2" in
    inc*)
        set_volume "$(($(get_volume) + 5))%"
        osd_send "  Volume: $(get_volume)%"
        ;;
    dec*)
        set_volume "$(($(get_volume) - 5))%"
        osd_send "  Volume: $(get_volume)%"
        ;;
    esac

elif [[ "$1" == "brightness" ]]; then
    case "$2" in
    inc*)
        change_b "5%+"
        osd_send " Brightness: $(get_b)%"
        send_brightness "$(get_b)"
        ;;
    dec*)
        change_b "5%-"
        osd_send " Brightness: $(get_b)%"
        send_brightness "$(get_b)"
        ;;
    esac
fi
