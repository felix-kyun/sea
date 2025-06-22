#!/usr/bin/env bash

echo > "${LOG_FILE}" # clear the log file
log () {
    (
        LOG_SOURCE="${BASH_SOURCE[1]}"
        source $(dirname "${BASH_SOURCE[0]}")/config.sh
        echo "$(date +"%H:%M:%S.%3N") [${1^^}] ${LOG_SOURCE}: ${@:2}" >> "${LOG_FILE}"
    ) & &>/dev/null
}
