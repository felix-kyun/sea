#!/usr/bin/env bash

notification_icon="󰎟 "
prev_notification=""

pad() {
    local padding_len="$1"
    local padding=""

    for ((i = 0; i < padding_len; i++)); do
        padding+=" "
    done

    echo -n "${padding}"
}

show_notification() {
    if [[ "${notification}" == "${prev_notification}" ]]; then
        return
    fi

    local buffer="\r\033[2K${notify_fg}"
    local main_len=$((${#notification} + 3))
    local main_buffer="${notification_icon}${notification}"
    local padding=""

    for ((i = 1; i <= main_len; i += 2)); do
        local show_buffer="${main_buffer::$i}"
        local padding_len=$(((COLS - i - 5) / 2))
        padding=$(pad "${padding_len}")

        echo -ne "${buffer}${padding}${show_buffer}${padding}"
        sleep 0.01
    done

    sleep "${notification_timeout:-5}"

    prev_notification="${notification}"
    notification=""
}
