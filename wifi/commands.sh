_launch() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

    kitty \
        --title sea-launcher \
        --config "${CURRENT_DIR}/wifi.conf" \
        -- bash -c "source ${CURRENT_DIR}/wifi.sh && main" &>/dev/null
}

