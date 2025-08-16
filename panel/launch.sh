#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
[[ -f "$HOME/.config/sea.sh" ]] && source "$HOME/.config/sea.sh"

kitten panel \
    --config "${CURRENT_DIR}/panel.conf" \
    --edge top \
    --layer top \
    --name sea-panel \
    --class sea-panel \
    --lines 1 \
    --detach \
    --output-name "${PANEL_OUTPUT}" \
    -- bash -c "while :; do ${CURRENT_DIR}/main.sh; done" \
    &>/tmp/sea-panel-launch.log
