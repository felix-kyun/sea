#!/usr/bin/env bash

handler_plugin_mouse_event() {
    local plugin_id="$1"
    local event="$2"

    log mouse "${event} on ${plugin_id}"

    [[ "${event}" == "press" ]] \
        && declare -f "${name}_onclick" &> /dev/null \
        && "${name}_onclick" "${id}"
}

handler_left() {
    event=$1
    x=$2
    keep_x=0
    for plugin in "${PLUGIN_LEFT[@]}"; do
        IFS=':' read name id <<< "${plugin}"
        plugin_x_end=$(( keep_x + "${id}_len" + 2 ));

        if (( x <= plugin_x_end && x >= keep_x )); then
            handler_plugin_mouse_event "${id}" "${event}"
        fi

        keep_x=$plugin_x_end
    done
}

handler_center() {
    event=$1
    x=$2
    keep_x=$(( left_len + left_padding_len ))
    for plugin in "${PLUGIN_CENTER[@]}"; do
        IFS=':' read name id <<< "${plugin}"
        plugin_x_end=$(( keep_x + "${id}_len" + 2 ));

        if (( x <= plugin_x_end && x >= keep_x )); then
            handler_plugin_mouse_event "${plugin}" "${event}"
        fi

        keep_x=$plugin_x_end
    done
}

handler_right() {
    event=$1
    x=$2
    keep_x=$(( left_len + left_padding_len + center_len + right_padding_len ))
    for plugin in "${PLUGIN_RIGHT[@]}"; do
        IFS=':' read name id <<< "${plugin}"
        plugin_x_end=$(( keep_x + "${id}_len" + 2 ));

        if (( x <= plugin_x_end && x >= keep_x )); then
            handler_plugin_mouse_event "${plugin}" "${event}"
        fi

        keep_x=$plugin_x_end
    done
}


mouse_handler() {
    local event="$1"
    IFS=';' read x y <<< "$2"

    if (( x >= 0 && x <= left_len )); then
        handler_left "${event}" "${x}"
    elif (( x >= (left_len + left_padding_len) && \
              x <= (left_len + left_padding_len + center_len) )); then
        handler_center "${event}" "${x}"
    elif (( x >= (left_len + left_padding_len + center_len + right_padding_len) && \
              x <= COLS )); then
        handler_right "${event}" "${x}"
    fi
}
