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
    main_len=$((${#notification}))
    main_buffer="${notification_icon}${notification}"
    padding=""

    delay_clear "${my_id}" &

    # show notification animation
    # no animation for osd
    if [[ "$2" == "osd" ]]; then
        padding_len=$(((COLS - main_len - 2) / 2))
        padding=$(pad "${padding_len}")

        echo -ne "${buffer}${padding}${notification}${padding}"
    else
        for ((i = 1; i <= main_len; i += 2)); do
            show_buffer="${main_buffer::$i}"
            padding_len=$(((COLS - i - 2) / 2))
            padding=$(pad "${padding_len}")

            echo -ne "${buffer}${padding}${show_buffer}${padding}"
            sleep 0.01
        done
    fi

}

delay_clear() {
    # send clear event after timeout with the id
    local my_id="$1"
    sleep "${notification_timeout}"
    echo "notify:clear:${my_id}" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
}
