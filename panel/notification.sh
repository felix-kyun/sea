#!/usr/bin/env bash

notification_icon="󰎟 "
notification_timeout="${notification_timeout:-5}"
notification_id=""

pad() {
    local padding_len="$1"
    local padding=""

    for ((i = 0; i < padding_len; i++)); do
        padding+=" "
    done

    echo -n "${padding}"
}

show_notification() {
    local my_id="$1"
    local buffer main_len main_buffer padding show_buffer padding_len

    buffer="\r\033[2K${notify_fg}"
    main_len=$((${#notification} + 3))
    main_buffer="${notification_icon}${notification}"
    padding=""

    # show notification animation
    for ((i = 1; i <= main_len; i += 2)); do
        # while rendering, if another notification comes, return
        [[ "${notification_id}" != "${my_id}" ]] && return

        show_buffer="${main_buffer::$i}"
        padding_len=$(((COLS - i - 5) / 2))
        padding=$(pad "${padding_len}")

        echo -ne "${buffer}${padding}${show_buffer}${padding}"
        sleep 0.01
    done

    # send clear event after timeout with the id
    {
        local my_id="$notification_id"
        sleep "${notification_timeout}"
        echo "notify:clear:${my_id}" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
    } &
}
