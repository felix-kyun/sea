#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

kitten panel \
    --config "${CURRENT_DIR}/panel.conf" \
    --edge top \
    --layer top \
    --name sea-panel \
    --class sea-panel \
    --lines 1 \
    -- bash -c "while :; do ${CURRENT_DIR}/main.sh; done" \
    &>/tmp/sea-panel-launch.log
