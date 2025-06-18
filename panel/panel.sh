#!/usr/bin/env bash

# exit on non zero status
# set -e 

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source ${CURRENT_DIR}/config.sh
source ${CURRENT_DIR}/utils.sh
source ${CURRENT_DIR}/../utils/formatting.sh

echo > "${LOG_FILE}" # clear the log file
log info "Starting Sea Panel"

# declare an associative array to hold plugin PIDs
plugin_pids=()

# enable debugging and tracing 
if [[ -n "$DEBUG" && "$DEBUG" == true ]]; then
    set -x
fi

# check plugins files
for plugin in "${PLUGINS[@]}"; do
    [[ -f "${CURRENT_DIR}/plugins/${plugin}.sh" ]] || {
        log error "Plugin file not found: ${CURRENT_DIR}/plugins/${plugin}.sh"
        exit 1
    }
done

# implement later
render() {
    log debug "Rendering Sea Panel"
    [[  -n "$DEBUG" && "$DEBUG" == true ]] || clear

    for plugin in "${PLUGINS[@]}"; do
        data_var="${plugin}_data"
        fg_var="${plugin}_fg"
        bg_var="${plugin}_bg"

        [[ -n "${plugin}_data" ]] && echo -ne "${!bg_var}${!fg_var} ${!data_var} ${RESET}"
    done
}

# do the first render 
echo -ne "${HIDE_CURSOR}"
render

cleanup() {
    log info "Cleaning up..."
    for pid in "${plugin_pids[@]}"; do
        log debug "Killing plugin with PID: $pid"
        kill "$pid" 2>/dev/null || true
    done
    log info "Sea Panel stopped."
    echo -ne "${SHOW_CURSOR}"
}

trap cleanup SIGINT SIGHUP SIGTERM EXIT

# start the plugins
for plugin in "${PLUGINS[@]}"; do
    
    {
        source "${CURRENT_DIR}/plugins/${plugin}.sh"
        declare ${plugin}_data=""
        declare ${plugin}_fg="${DEFAULT}"
        declare ${plugin}_bg="${DEFAULT_BG}"

        # wait for the socket to be ready
        sleep 0.1

        # exec onload if it exists
        declare -f ${plugin}_onload &> /dev/null \
        && ${plugin}_onload "${plugin}" 

        # start the plugin
        ${plugin}_start "${plugin}" 
    } &

    log debug "Started plugin: ${plugin} with PID: ${!}"
    echo $! >> /tmp/sea_panel_override.pids
    plugin_pids+="${!}"
done

# start the socket listener
socat -u UNIX-LISTEN:"$SOCKET",fork - | while read msg; do
    if [[ "$msg" == "exit" ]]; then
        log info "Exit command received, stopping Sea Panel."
        cleanup
        exit 0
    fi

    IFS=':' read -r plugin_id event data <<< "$msg"
    log debug "received ${plugin_id}(${event}, ${#data}): $(echo ${data} | sed 's/\x1b/\\e/g')"
    eval "${plugin_id}_data"='${data}' # update the plugin data 
    
    case "$event" in 
        "update")
            eval "${plugin_id}_data"='${data}' # update the plugin data 
            ;;
        "fg")
            eval "${plugin_id}_fg"='${data}' # update the plugin data 
            ;;
        "bg")
            eval "${plugin_id}_bg"='${data}' # update the plugin data 
            ;;
        *)
            log warning "Unknown event: ${event} for plugin: ${plugin_id}"
            ;;
    esac

    render
done
