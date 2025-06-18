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
    clear
    echo -ne "${HIDE_CURSOR}${BOLD}${RED}sea panel"
}

# do the first render 
render

cleanup() {
    log info "Cleaning up..."
    for pid in "${plugin_pids[@]}"; do
        log debug "Killing plugin with PID: $pid"
        kill "$pid" 2>/dev/null || true
    done
    log info "Sea Panel stopped."
}

trap cleanup SIGINT SIGHUP SIGTERM EXIT

# start the plugins
for plugin in "${PLUGINS[@]}"; do
    sleep 1 # give some time for the panel to start
    
    # start a subshell for each plugin
    (
        source "${CURRENT_DIR}/plugins/${plugin}.sh"

        # exec onload if it exists
        declare -f ${plugin}_onload &> /dev/null \
        && ${plugin}_onload "${plugin}" 

        # start the plugin
        ${plugin}_start "${plugin}" 
    ) &

    log debug "Started plugin: ${plugin} with PID: ${!}"
    echo $! >> /tmp/sea_panel_override.pids
    plugin_pids+="${!}"
done

# start the socket listener
socat -u UNIX-LISTEN:"$SOCKET",fork - | while read msg; do
    log debug "Received message: $msg"
    if [[ "$msg" == "exit" ]]; then
        log info "Exit command received, stopping Sea Panel."
        cleanup
        exit 0
    fi
    render
done
