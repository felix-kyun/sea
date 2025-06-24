#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source "${CURRENT_DIR}/../utils/fzf_theme_override.sh"
actions=(
    " Power Off:systemctl poweroff"
    "󰜉 Reboot:systemctl reboot"
    "󰒲 Suspend:systemctl suspend"
    "󰋊 Hibernate:systemctl hibernate"
    "󰌾 Lock:loginctl lock-session ${XDG_SESSION_ID}"
    "󰍃 Logout:loginctl terminate-session ${XDG_SESSION_ID}"
)

selected=$(printf '%s\n' "${actions[@]}" \
    | fzf      \
    --reverse   \
    --no-multi   \
    --border none \
    --no-scrollbar \
    --delimiter ':' \
    --layout reverse \
    --with-nth '{1}'  \
    --accept-nth 2)

if [[ -n "$selected" ]]; then
        setsid bash -c "${selected}" < /dev/null > /dev/null 2>&1 &
fi
