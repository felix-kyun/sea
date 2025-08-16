_launch() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

    kitty \
        --title sea-emoji \
        --config ${CURRENT_DIR}/emoji.conf \
        -- bash -c "source ${CURRENT_DIR}/utils.sh && show_emoji_picker" &>/dev/null
}
