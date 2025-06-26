#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

kitty \
    --title sea-clipboard \
    --config ${CURRENT_DIR}/clipboard.conf \
    -- bash -c "source ${CURRENT_DIR}/clipboard.sh && cliphist-fzf-copy" &>/dev/null
