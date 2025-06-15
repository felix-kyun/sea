#!/bin/bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

kitty \
    --title sea-shell \
    --config ${CURRENT_DIR}/launcher.conf \
    -- bash -c "source ${CURRENT_DIR}/utils.sh && show_launcher"
