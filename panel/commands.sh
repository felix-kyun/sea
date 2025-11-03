_create() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
    [[ -f "$HOME/.config/sea.conf" ]] && source "$HOME/.config/sea.conf"

    kitten panel \
        --config "${CURRENT_DIR}/panel.conf" \
        --edge top \
        --layer top \
        --name sea-panel \
        --class sea-panel \
        --lines 1 \
        --output-name "${PANEL_OUTPUT}" \
        -- "${CURRENT_DIR}/build/sea_panel_main"
}

_debug_mode() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
    [[ -f "$HOME/.config/sea.conf" ]] && source "$HOME/.config/sea.conf"

    kitten panel \
        --config "${CURRENT_DIR}/panel.conf" \
        --edge top \
        --layer top \
        --name sea-panel \
        --class sea-panel \
        --lines 7 \
        --output-name "${PANEL_OUTPUT}" \
        -- bash -c "
             strace ${CURRENT_DIR}/build/sea_panel_main --stdout;
             read -r -p 'Press any key to exit'
    "
}

_launch() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")

    "${CURRENT_DIR}/build/sea_panel_launcher"
}

_build() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
    cd "$CURRENT_DIR" || exit

    make clean
    make -j release
}

_kill() {
    pkill -f sea_panel_launcher
    pkill -f sea_panel_main
}

_notify() {
    echo -n "$1" > /tmp/sea-notify-pipe
}

_osd() {
    CURRENT_DIR="$(dirname "${BASH_SOURCE[0]}")"

    bash "${CURRENT_DIR}/osd.sh" "$@"
}
