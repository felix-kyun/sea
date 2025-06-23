#!/usr/bin/env bash

handler_left() {
    x=$1
    keep_x=0
    for plugin in "${PLUGIN_LEFT[@]}"; do
        IFS=':' read name id <<< "${plugin}"
        plugin_x_end=$(( keep_x + "${id}_len" + 2 ));

        if (( x <= plugin_x_end && x >= keep_x )); then
            log mouse "clicked on ${name} at ${x}"
            declare -f "${name}_onclick" &> /dev/null \
                && "${name}_onclick" "${id}"
            return 0
        fi

        keep_x=$plugin_x_end
    done
}

handler_center() {
    x=$1
    keep_x=$(( left_len + left_padding_len ))
    for plugin in "${PLUGIN_CENTER[@]}"; do
        IFS=':' read name id <<< "${plugin}"
        plugin_x_end=$(( keep_x + "${id}_len" + 2 ));

        if (( x <= plugin_x_end && x >= keep_x )); then
            log mouse "clicked on ${name} at ${x}"
            declare -f "${name}_onclick" &> /dev/null \
                && "${name}_onclick" "${id}"
            return 0
        fi

        keep_x=$plugin_x_end
    done
}

handler_right() {
    x=$1
    keep_x=$(( left_len + left_padding_len + center_len + right_padding_len ))
    for plugin in "${PLUGIN_RIGHT[@]}"; do
        IFS=':' read name id <<< "${plugin}"
        plugin_x_end=$(( keep_x + "${id}_len" + 2 ));

        if (( x <= plugin_x_end && x >= keep_x )); then
            log mouse "clicked on ${name} at ${x}"
            declare -f "${name}_onclick" &> /dev/null \
                && "${name}_onclick" "${id}"
            return 0
        fi

        keep_x=$plugin_x_end
    done
}


mouse_handler() {
    local event="$1"
    IFS=';' read x y <<< "$2"

    log mouse "${event} at (${x}, ${y})"
    [[ "${event}" != "press" ]] && return 0;

    if (( x >= 0 && x <= left_len )); then
        log mouse "Left area clicked at ${x}"
        handler_left "${x}"
    elif (( x >= (left_len + left_padding_len) && \
              x <= (left_len + left_padding_len + center_len) )); then
        log mouse "Center area clicked at ${x}"
        handler_center "${x}"
    elif (( x >= (left_len + left_padding_len + center_len + right_padding_len) && \
              x <= COLS )); then
        log mouse "Right area clicked at ${x}"
        handler_right "${x}"
    fi
}
