#!/usr/bin/env bash
song_limit=${song_limit:-50}
song_default="󰎆 No song playing"
song_format="${song_format:-'{{ title }}'}"

song_fetch() {
    stdbuf -oL playerctl metadata -f "$song_format" -F
}

song_start() {
    song_fetch | while read -r song; do
        if [[ -z "$song" ]]; then
            send update "${song_default}"
        else
            song=$(echo "${song}" | tr -cd '\0-\177')

            # filter out unwanted stuff
            if [[ ${#song_modifiers[@]} -gt 0 ]]; then
                for modifier in "${song_modifiers[@]}"; do
                    song=$(echo "$song" | sed -E "s/${modifier}/g")
                done
            fi

            # limit string length
            if [[ ${#song} -gt ${song_limit} ]]; then
                song="${song:0:${song_limit}}..."
            fi
            send update "󰎆 ${song}"
        fi
    done
}

song_on_click() {
    playerctl play-pause
}

song_on_scrollup() {
    playerctl next
}

song_on_scrolldown() {
    playerctl previous
}
