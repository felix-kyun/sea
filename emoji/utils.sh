#!/usr/bin/env bash

CACHE_FILE="${HOME}/.cache/sea/emoji.cache.txt"
EMOJI_URL="https://unicode.org/Public/emoji/latest/emoji-test.txt"

# get from cache or fetch cache and return
get_emoji_list() {
    # create cache dir
    mkdir -p "$(dirname "$CACHE_FILE")"

    # check if theres cache
    if [[ -s "$CACHE_FILE" ]]; then
        cat "$CACHE_FILE"
    else
        curl -s "$EMOJI_URL" \
            | grep -Ev '^$|^#' \
            | grep 'fully-qualified' \
            | sed 's/.*# //g; s/E[0-9.]* //g' \
        > "$CACHE_FILE"
        cat "$CACHE_FILE"
    fi
}

show_emoji_picker() {
    local selected_emoji=$(get_emoji_list \
                            | fzf --delimiter ' ' --accept-nth 1)

    [[ -n "$selected_emoji" ]] && \
        wl-copy "$selected_emoji" 2>/dev/null
}
