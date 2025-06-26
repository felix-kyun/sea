#!/bin/bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

kitty \
    --title sea-launcher \
    --config ${CURRENT_DIR}/bluetooth.conf \
    -- bash -c "source ${CURRENT_DIR}/bluetooth.sh && main_menu" &>/dev/null

