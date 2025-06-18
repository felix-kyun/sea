#!/usr/bin/env bash

debug () {
    if [ "$DEBUG" = true ]; then
        gum log --level debug "$@"
    fi
}

debug_exec() {
    if [ "$DEBUG" = true ]; then
        gum log --level debug $("$@")
    fi
}

log () {
    LOG_SOURCE="${BASH_SOURCE[1]}"
    source $(dirname "${BASH_SOURCE[0]}")/config.sh
    gum log --level $1 --time Timeonly -o "${LOG_FILE}" "${LOG_SOURCE}: ${@:2}"
    debug "${1}: ${LOG_SOURCE}: ${@:2}"
}

send () {
    echo -e "${PLUGIN_ID}:${1}:${@:2}" | socat - UNIX-CONNECT:"${SOCKET}" 
    sleep 0.1 # give some time for the socket to process the message
}

