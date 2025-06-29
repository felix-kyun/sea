#!/usr/bin/env bash

# rendering part

render() {
    COLS=$(tput cols)

    # maintain seperate buffers
    buffer_main="\r\033[2K${PANEL_BG}"
    buffer_left=""
    buffer_right=""
    buffer_center=""

    # if notification is set, only render that and exit
    if [[ -n "${notification}" ]]; then
        padding_len=$((COLS - ${#notification} - 3)) # -3 for 󰎟 and spaces

        for ((i = 0; i < (padding_len / 2); i++)); do
            buffer_main+=" "
        done

        buffer_main+="${notify_fg} 󰎟 ${notification}"

        for ((i = 0; i < (padding_len / 2); i++)); do
            buffer_main+=" "
        done

        echo -ne "${buffer_main}"
        return
    fi

    # render the left side
    left_len=0
    for plugin in "${PLUGIN_LEFT[@]}"; do
        IFS=':' read -r name id <<<"${plugin}"

        buffer_left+=$(process_plugin "${id}")
        plugin_len=${id}_len
        left_len=$((left_len + ${!plugin_len} + 2))
    done

    # render the center side
    center_len=0
    for plugin in "${PLUGIN_CENTER[@]}"; do
        IFS=':' read -r name id <<<"${plugin}"

        buffer_center+=$(process_plugin "${id}")
        plugin_len=${id}_len
        center_len=$((center_len + ${!plugin_len} + 2))
    done

    # render the right side
    right_len=0
    for plugin in "${PLUGIN_RIGHT[@]}"; do
        IFS=':' read -r name id <<<"${plugin}"

        buffer_right+=$(process_plugin "${id}")
        plugin_len=${id}_len
        right_len=$((right_len + ${!plugin_len} + 2))
    done

    # calculate padding
    left_padding_len=$((((COLS - center_len) / 2) - left_len))
    right_padding_len=$(((COLS - center_len - left_len - left_padding_len) - right_len))

    # left padding
    left_padding=""
    for ((i = 0; i < left_padding_len; i++)); do
        left_padding+=" "
    done

    # right padding
    right_padding=""
    for ((i = 0; i < right_padding_len; i++)); do
        right_padding+=" "
    done

    echo -ne "${buffer_main}${buffer_left}${left_padding}${buffer_center}${right_padding}${buffer_right}"
}
