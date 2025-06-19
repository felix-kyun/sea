#!/usr/bin/env bash

log () {
    (
        LOG_SOURCE="${BASH_SOURCE[1]}"
        source $(dirname "${BASH_SOURCE[0]}")/config.sh
        echo "$(date +"%H:%M:%S.%3N") [${1^^}] ${LOG_SOURCE}: ${@:2}" >> "${LOG_FILE}"
    ) & &>/dev/null
}

send () {
    echo -e "${PLUGIN_ID}:${1}:${@:2}" | socat - UNIX-CONNECT:"${SOCKET}" 
}

getid() {
    IFS=':' read name id <<< "$1"

    if [[ -z "${id}" ]]; then 
        echo ${name}
    else
        echo ${id}
    fi
}

getname() {
    IFS=':' read name id <<< "$1"
    echo ${name}
}
