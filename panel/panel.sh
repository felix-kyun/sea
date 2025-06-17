#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source ${CURRENT_DIR}/config.sh
source ${CURRENT_DIR}/utils.sh
source ${CURRENT_DIR}/../utils/formatting.sh

echo > "${LOG_FILE}" # clear the log file
log info "Starting Sea Panel"

# enable debugging and tracing 
if [[ -n "$DEBUG" && "$DEBUG" == true ]]; then
    set -x
fi

# source plugins
for plugin in "${PLUGINS[@]}"; do
    log debug "Loading plugin: ${plugin}"
    source "${CURRENT_DIR}/plugins/${plugin}.sh"
    if [[ $? -ne 0 ]]; then
        log error "Failed to load plugin: ${plugin}"
        exit 1
    fi
    log info "Plugin ${plugin} loaded successfully"
done

# start the plugins
for plugin in "${PLUGINS[@]}"; do
    # onload
    declare -f "${plugin}_onload" >> /dev/null && {
        log debug "Executing onload for plugin: ${plugin}"
        ${plugin}_onload

        if [[ $? -ne 0 ]]; then
            log error "Failed to execute onload for plugin: ${plugin}"
            exit 1
        fi

    }

    # start 
    declare -f "${plugin}_start" > /dev/null && {
        log info "Starting plugin: ${plugin}"
        ${plugin}_start 

        if [[ $? -ne 0 ]]; then
            log error "Failed to start plugin: ${plugin}"
            exit 1
        fi

        log success "Plugin ${plugin} started successfully"
    } &
done

