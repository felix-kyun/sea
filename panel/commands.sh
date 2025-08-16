_get_pid() {
    ps aux | awk "/kitty \+kitten panel.*sea\/panel\/main\.sh/ { print \$2 }"
}

_kill() {
    kill "$(_get_pid)"
}

_launch() {
    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
    [[ -f "$HOME/.config/sea.conf" ]] && source "$HOME/.config/sea.conf"

    kitten panel \
        --config "${CURRENT_DIR}/panel.conf" \
        --edge top \
        --layer top \
        --name sea-panel \
        --class sea-panel \
        --lines 1 \
        --detach \
        --output-name "${PANEL_OUTPUT}" \
        -- bash -c "while :; do ${CURRENT_DIR}/main.sh; done" \
        &>/tmp/sea-panel-launch.log

}

_monitor() {
    local interval
    local process_pid

    interval="${1:-5}"
    process_pid=$(_get_pid)

    [[ -z "$process_pid" ]] && {
        echo "Panel process not found."
        exit 1
    }

    echo "Sea Panel (PID: $process_pid)"
    while true; do
        pids=$(pstree -p "$process_pid" | grep -oP '\(\K[0-9]+' | tr '\n' ',' | sed 's/,$//')
        cpu_usage=$(ps --no-headers -o %cpu -p "$pids" | awk '{s+=$1} END {print s}')
        mem_usage=$(ps --no-headers -o %mem -p "$pids" | awk '{s+=$1} END {print s}')
        echo -ne "\rCPU Usage: $cpu_usage% | Memory Usage: $mem_usage%    "

        sleep "$interval"
    done
}

_notify() {
    CURRENT_DIR="$(dirname "${BASH_SOURCE[0]}")"

    source "${CURRENT_DIR}/utils.sh"
    source "${CURRENT_DIR}/config.sh"

    echo "notify:set:${1}" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
}

_osd() {
    CURRENT_DIR="$(dirname "${BASH_SOURCE[0]}")"

    bash "${CURRENT_DIR}/osd.sh" "$@"
}
