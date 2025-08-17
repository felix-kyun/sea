_launch() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

    kitty \
        --title sea-clipboard \
        --config ${CURRENT_DIR}/clipboard.conf \
        -- bash -c "source ${CURRENT_DIR}/clipboard.sh && cliphist-fzf-copy" &>/dev/null
}

_listen() {
    wl-paste --watch cliphist -max-items=1000000 store
}
