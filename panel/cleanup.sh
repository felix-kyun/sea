#!/usr/bin/env bash

cleanup() {
    log info "Unloading plugins..."
    for plugin in "${PLUGINS[@]}"; do
        IFS=':' read name id <<< "${plugin}"

        # check if the plugin has a unload function
        if declare -f ${name}_unload &> /dev/null; then
            log debug "Unloading plugin: ${name}:${id}"
            ${name}_unload "${id}" 
        fi
    done

    log info "Cleaning up..."
    while IFS= read -r pid; do
        if [[ -n "$pid" && -e /proc/$pid ]]; then
            log debug "Killing process with PID: $pid"
            kill "$pid" 2>/dev/null || true
        fi
    done < "${PID_FILE}"

    log info "Sea Panel stopped."
    echo -ne "${SHOW_CURSOR}"
    printf '\033[?1000l'
    printf '\033[?1006l'
    printf '\033[?1002l'
    stty sane

    exit 0
}

