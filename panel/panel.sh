#!/usr/bin/env bash

# exit on non zero status
# set -e 

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source ${CURRENT_DIR}/config.sh
source ${CURRENT_DIR}/utils.sh
source ${CURRENT_DIR}/../utils/formatting.sh

echo > "${LOG_FILE}" # clear the log file
log info "Starting Sea Panel"

# declare an array to hold plugin PIDs
plugin_pids=()

# enable debugging and tracing 
if [[ -n "$DEBUG" && "$DEBUG" == true ]]; then
    set -x
fi

# check plugins files
tmp_plugins_names=()
for plugin in "${PLUGINS[@]}"; do
    id=$(getid "${plugin}")
    name=$(getname "${plugin}")
    tmp_plugins_names+=("${name}:${id}")

    [[ -f "${CURRENT_DIR}/plugins/${name}.sh" ]] || {
        log error "Plugin file not found: ${CURRENT_DIR}/plugins/${name}.sh"
        exit 1
    }
done
PLUGINS=("${tmp_plugins_names[@]}") 

log debug "Plugins loaded: ${tmp_plugins_names[*]}"

# implement later
render() {
    # log debug "Rendering Sea Panel"
    echo -ne "\r\033[2K"
    buffered_output=""

    for plugin in "${PLUGINS[@]}"; do
        IFS=':' read name id <<< "${plugin}"

        data_var="${id}_data"
        fg_var="${id}_fg"
        bg_var="${id}_bg"

        buffered_output+="${!bg_var}${!fg_var} ${!data_var} ${RESET} "
    done
    echo -ne "${buffered_output}"
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
    declare ${id}_fg="${DEFAULT}"
    declare ${id}_bg="${DEFAULT_BG}"
    
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
socat -u UNIX-LISTEN:"$SOCKET",fork - | while read msg; do
    exec 200>${LOCK_FILE}
    flock 200

    IFS=':' read -r plugin_id event data <<< "$msg"
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
