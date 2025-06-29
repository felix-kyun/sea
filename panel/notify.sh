#!/usr/bin/env bash

# helper script to send notifications to the panel
CURRENT_DIR="$(dirname "${BASH_SOURCE[0]}")"

source "${CURRENT_DIR}/utils.sh"
source "${CURRENT_DIR}/config.sh"

echo "notify:set:${1}" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
sleep "${notification_timeout:-5}"
echo "notify:clear:" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
