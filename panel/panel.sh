#!/usr/bin/env bash

# exit on non zero status
# set -e 

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source ${CURRENT_DIR}/config.sh
source ${CURRENT_DIR}/utils.sh

[[ -e "${LOCK_FILE}" ]] && rm -f "${LOCK_FILE}"
[[ -e "${PIPE_FILE}" ]] && rm -f "${PIPE_FILE}"

echo > "${LOG_FILE}" 
log info "Starting Sea Panel"
mkfifo "${PIPE_FILE}"
exec 99<> "${PIPE_FILE}" || { log error "Failed to open pipe file: ${PIPE_FILE}"; exit 1; }

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

# rendering part
COLS=$(tput cols)
render() {
    # maintain seperate buffers
    buffer_main="\r\033[2K${PANEL_BG}"
    buffer_left=""
    buffer_right=""
    buffer_center=""

    # render the left side
    for plugin in "${PLUGIN_LEFT[@]}"; do
        buffer_left+=$(process_plugin)
    done
    left_len=$(true_length "${buffer_left}")

    # render the center side
    for plugin in "${PLUGIN_CENTER[@]}"; do
        buffer_center+=$(process_plugin)
    done
    center_len=$(true_length "${buffer_center}")

    # render the right side
    for plugin in "${PLUGIN_RIGHT[@]}"; do
        buffer_right+=$(process_plugin)
    done
    right_len=$(true_length "${buffer_right}")

    # calculate padding 
    left_padding_len=$(( ((COLS - center_len ) / 2) - left_len ))
    right_padding_len=$(( (COLS - center_len - left_len - left_padding_len) - right_len ))

    # left padding
    left_padding=""
    for ((i=0; i<left_padding_len; i++)); do
        left_padding+=" "
    done

    # right padding
    right_padding=""
    for ((i=0; i<right_padding_len; i++)); do
        right_padding+=" "
    done

    echo -ne "${buffer_main}${buffer_left}${left_padding}${buffer_center}${right_padding}${buffer_right}"
}

# do the first render 
reset
echo -ne "${HIDE_CURSOR}"
render

cleanup() {
    log info "Unloading plugins..."
    for plugin in "${PLUGINS[@]}"; do
        IFS=':' read name id <<< "${plugin}"

        # check if the plugin has a unload function
        if declare -f ${name}_unload &> /dev/null; then
            log debug "Unloading plugin: ${name}:${id}"
            ${name}_unload "${id}" 
        fi
    done

    log info "Cleaning up..."
    for pid in "${plugin_pids[@]}"; do
        log debug "Killing plugin with PID: $pid"
        kill "$pid" 2>/dev/null || true
    done

    log info "Sea Panel stopped."
    echo -ne "${SHOW_CURSOR}"
}

trap cleanup SIGINT SIGHUP

# start the plugins
for plugin in "${PLUGINS[@]}"; do
    IFS=':' read name id <<< "${plugin}"

    source "${CURRENT_DIR}/plugins/${name}.sh"
    default_data="${name}_default"
    declare ${id}_data="${!default_data}"
    
    {
        # exec onload if it exists
        declare -f ${name}_onload &> /dev/null \
        && ${name}_onload "${id}" 

        # start the plugin
        sleep 0.1 # give some time for the plugin to load
        ${name}_start "${id}" 
    } &

    plugin_pids+=("${!}")
    # log after storing as logger uses subshell so it changes $!
    log debug "Started plugin: ${name}:${id} with PID: ${!}"
done

while :; do
    while IFS=':' read -r plugin_id event data < ${PIPE_FILE}; do
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
    done
done
