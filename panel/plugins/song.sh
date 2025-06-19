#!/usr/bin/env bash

song_fetch() {
    playerctl \
        -p plasma-browser-integration,firefox,chromium,chrome,instance metadata \
        -f '{{ artist }} - {{ title }}' -F 
}

song_start() {
    PLUGIN_ID=$1
    send bg "${BLUE_BG}"
    send fg "${BLACK}${BOLD}"

    song_fetch | while read song; do 
        if [[ -z "$song" ]]; then
            send update "󰎆 No song playing"
        else
            send update "󰎆 ${song}"
        fi
    done
}
