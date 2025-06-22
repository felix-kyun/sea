#!/usr/bin/env bash

# rendering part
COLS=$(tput cols)

render() {
    # maintain seperate buffers
    buffer_main="\r\033[2K${PANEL_BG}"
    buffer_left=""
    buffer_right=""
    buffer_center=""

    # render the left side
    for plugin in "${PLUGIN_LEFT[@]}"; do
        buffer_left+=$(process_plugin)
    done
    left_len=$(true_length "${buffer_left}")

    # render the center side
    for plugin in "${PLUGIN_CENTER[@]}"; do
        buffer_center+=$(process_plugin)
    done
    center_len=$(true_length "${buffer_center}")

    # render the right side
    for plugin in "${PLUGIN_RIGHT[@]}"; do
        buffer_right+=$(process_plugin)
    done
    right_len=$(true_length "${buffer_right}")

    # calculate padding 
    left_padding_len=$(( ((COLS - center_len ) / 2) - left_len ))
    right_padding_len=$(( (COLS - center_len - left_len - left_padding_len) - right_len ))

    # left padding
    left_padding=""
    for ((i=0; i<left_padding_len; i++)); do
        left_padding+=" "
    done

    # right padding
    right_padding=""
    for ((i=0; i<right_padding_len; i++)); do
        right_padding+=" "
    done

    echo -ne "${buffer_main}${buffer_left}${left_padding}${buffer_center}${right_padding}${buffer_right}"
}

