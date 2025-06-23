#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

kitten panel \
        --config ${CURRENT_DIR}/panel.conf \
        --edge top \
        --name sea-panel \
        --class sea-panel \
        --lines 1 \
        -- bash ${CURRENT_DIR}/main.sh \
        # --margin-top 5 \
        # --margin-left 30 \
        # --margin-right 30 \
