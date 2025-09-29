#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source "${CURRENT_DIR}/utils.sh"

include config.sh
include log.sh
include user_config_loader.sh
include render.sh
include plugins.sh
include cleanup.sh
include event_reader.sh
include panel.sh
include mouse_handler.sh
include notification.sh

reset

# ensure log and pid file are empty
echo >"${PID_FILE}"
echo >"${LOG_FILE}"

# enable terminal mouse events
echo -ne "${HIDE_CURSOR}"
printf '\e[?1000h'
printf '\e[?1006h'
printf '\e[?1002h'
stty -echo -icanon time 0 min 1

# trap
trap cleanup SIGINT SIGHUP

# start
log info "Starting Sea Panel"

# enable debugging and tracing
if [[ -n "$DEBUG" && "$DEBUG" == true ]]; then
    set -x
fi

# process plugin entries
# basically adds plugin id if missing
transform PLUGIN_LEFT
transform PLUGIN_RIGHT
transform PLUGIN_CENTER
PLUGINS=()

for plugin in "${PLUGIN_LEFT[@]}" "${PLUGIN_RIGHT[@]}" "${PLUGIN_CENTER[@]}"; do
    IFS=':' read -r name id <<<"${plugin}"
    PLUGINS+=("${name}:${id}")
done

[[ -e "${LOCK_FILE}" ]] && rm -f "${LOCK_FILE}"
[[ -e "${SOCKET}" ]] && rm -f "${SOCKET}"

panel_loop &
echo "$!" >"${PID_FILE}"

event_reader
