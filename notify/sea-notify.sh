#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

kitty \
    --title sea-notify \
    --config ${CURRENT_DIR}/sea-notify.conf \
    -- bash -c "${CURRENT_DIR}/notification.sh '${1}' '${2}' '${3}' '${4}' '${5}' " \
