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
        -- bash -c "./build/panel" 
}
