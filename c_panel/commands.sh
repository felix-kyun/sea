_launch() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
    [[ -f "$HOME/.config/sea.conf" ]] && source "$HOME/.config/sea.conf"

    kitten panel \
        --config "${CURRENT_DIR}/panel.conf" \
        --edge top \
        --layer top \
        --name sea-panel \
        --class sea-panel \
        --lines 1 \
        --detach \
        --output-name "${PANEL_OUTPUT}" \
        -- "${CURRENT_DIR}/build/panel"
}

_build() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
    cd "$CURRENT_DIR" || exit

    make clean
    make -j release
}

_kill() {
    ps aux | grep 'build/panel' | awk ' $11 ~ /^\/.*build\/panel$/ { print $2 }' | xargs kill -9
}

_notify() {
    echo -n "$1" > /tmp/sea-notify-pipe
}

_osd() {
    CURRENT_DIR="$(dirname "${BASH_SOURCE[0]}")"

    bash "${CURRENT_DIR}/osd.sh" "$@"
}
