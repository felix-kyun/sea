#!/usr/bin/env bash

# exit on non zero status
# set -e 

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source "${CURRENT_DIR}/utils.sh"

include config.sh
include render.sh
include plugins.sh
include cleanup.sh
include log.sh

reset
echo -ne "${HIDE_CURSOR}"
trap cleanup SIGINT SIGHUP
log info "Starting Sea Panel"

# declare an array to hold plugin PIDs
plugin_pids=()

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
    IFS=':' read name id <<< "${plugin}"
    PLUGINS+=("${name}:${id}")
done

# start the plugins
for plugin in "${PLUGINS[@]}"; do
    IFS=':' read name id <<< "${plugin}"

    source "${CURRENT_DIR}/plugins/${name}.sh"
    default_data="${name}_default"
    declare ${id}_data="${!default_data}"
    
    {
        # wait for the socket to be ready
        sleep 0.1

        # exec onload if it exists
        declare -f ${name}_onload &> /dev/null \
        && ${name}_onload "${id}" 

        # start the plugin
        ${name}_start "${id}" 
    } &

    plugin_pids+=("${!}")
    # log after storing as logger uses subshell so it changes $!
    log debug "Started plugin: ${name}:${id} with PID: ${!}"
done

# start the socket listener
[[ -e "${LOCK_FILE}" ]] && rm -f "${LOCK_FILE}"
[[ -e "${SOCKET}" ]] && rm -f "${SOCKET}"

socat -u UNIX-LISTEN:"$SOCKET",fork - 2>/dev/null \
    | while IFS=':' read -r plugin_id event data; do

    exec 200>${LOCK_FILE}
    flock 200

    log debug "received ${plugin_id}(${event}, ${#data}): $(echo ${data} | sed 's/\x1b/\\e/g')"
    
    case "$event" in 
        "update")
            eval "${plugin_id}_data"='${data}'  
            ;;
        "fg")
            eval "${plugin_id}_fg"='${data}'  
            ;;
        "bg")
            eval "${plugin_id}_bg"='${data}'  
            ;;
        *)
            log warning "Unknown event: ${event} for plugin: ${plugin_id}"
            ;;
    esac

    render
    exec 200>&- 
done
