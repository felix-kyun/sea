_launch() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

    kitty \
        --title sea-powermenu \
        --config ${CURRENT_DIR}/powermenu.conf \
        -- bash -c "${CURRENT_DIR}/powermenu.sh" &>/dev/null
}
