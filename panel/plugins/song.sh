#!/usr/bin/env bash
song_limit=40
song_default="󰎆 No song playing"
song_format="{{ title }}"
song_filters=(
    "\( -\)\? YouTube Music$"
    )

song_fetch() {
    stdbuf -oL playerctl metadata -f "$song_format" -F
}

song_start() {
    PLUGIN_ID=$1

    song_fetch | while read song; do 
        if [[ -z "$song" ]]; then
            send update "${song_default}"
        else
            song=$(echo "${song}" | tr -cd '\0-\177')

            # filter out unwanted stuff
            if [[ ${#song_filters[@]} -gt 0 ]]; then
                for filter in "${song_filters[@]}"; do
                    song=$(echo "$song" | sed "s/${filter}//g")
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
