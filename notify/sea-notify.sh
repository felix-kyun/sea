#!/usr/bin/env bash

## uncomment this to show popup on screen
# CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
# kitty \
#     --title sea-notify \
#     --config ${CURRENT_DIR}/sea-notify.conf \
#     -- bash -c "${CURRENT_DIR}/notification.sh '${1}' '${2}' '${3}' '${4}' '${5}' " \

app_name=$1
summary=$2
body=$3

sea panel notify \
    "${HIDE_CURSOR}${BOLD}${CYAN}${app_name}${WHITE} ${summary}${RESET}: ${body}${RESET}"
