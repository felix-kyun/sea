#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

kitten panel \
        --config ${CURRENT_DIR}/panel.conf \
        --edge top \
        --name sea-panel \
        -- bash ${CURRENT_DIR}/panel.sh
