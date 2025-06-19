#!/usr/bin/env bash

song_fetch() {
    playerctl \
        -p plasma-browser-integration,firefox,chromium,chrome,instance metadata \
        -f '{{ title }}' -F
        # -f '{{ artist }} - {{ title }}' -F 
}

song_start() {
    PLUGIN_ID=$1
    send bg "${BLUE_BG}"
    send fg "${BLACK}${BOLD}"

    song_fetch | while read song; do 
        if [[ -z "$song" ]]; then
            send update "󰎆 No song playing"
        else
            song=$(echo "${song}" | tr -cd '\0-\177')
            if [[ ${#song} -gt 30 ]]; then
                song="${song:0:30}..."
            fi
            send update "󰎆 ${song}"
        fi
    done
}
