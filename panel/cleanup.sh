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
    for pid in "${plugin_pids[@]}"; do
        log debug "Killing plugin with PID: $pid"
        kill "$pid" 2>/dev/null || true
    done

    log info "Sea Panel stopped."
    echo -ne "${SHOW_CURSOR}"
}

