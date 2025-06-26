#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source "${CURRENT_DIR}/../utils/fzf_theme_override.sh"
actions=(
    "󰒲 Suspend:systemctl suspend"
    " Power Off:systemctl poweroff"
    "󰜉 Reboot:systemctl reboot"
    "󰋊 Hibernate:systemctl hibernate"
    "󰌾 Lock:loginctl lock-session ${XDG_SESSION_ID}"
    "󰍃 Logout:loginctl terminate-session ${XDG_SESSION_ID}"
)

launch_detached() {
  (
    setsid bash -c "$1" < /dev/null > /dev/null 2>&1 &
  ) &
}


selected=$(printf '%s\n' "${actions[@]}" \
    | fzf     \
    --no-info  \
    --reverse   \
    --no-multi   \
    --border none \
    --no-scrollbar \
    --delimiter ':' \
    --layout reverse \
    --with-nth '{1}'  \
    --accept-nth 2)

if [[ -n "$selected" ]]; then
    launch_detached "${selected}"
    sleep 0.1
fi
