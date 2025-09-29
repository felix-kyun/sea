#!/usr/bin/env bash

log() {
    # disable logging when $LOG set to false
    [[ $LOG != true ]] && return

    (
        source "$(dirname "${BASH_SOURCE[0]}")/config.sh"
        echo "$(date +"%H:%M:%S.%3N") [${1^^}]: ${*:2}" >>"${LOG_FILE}"
    ) &>/dev/null &
}
