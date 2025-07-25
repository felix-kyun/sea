panel_loop() {
    # start the plugins
    for plugin in "${PLUGINS[@]}"; do
        IFS=':' read -r name id <<<"${plugin}"

        source "${CURRENT_DIR}/plugins/${name}.sh"
        default_data="${name}_default"
        declare "${id}_data"="${!default_data}"
        declare "${id}_len"="$(true_length "${!default_data}")"

        {
            # wait for the socket to be ready
            sleep 0.5

            # exec onload if it exists
            declare -f "${name}_onload" &>/dev/null &&
                "${name}_onload"

            # start the plugin
            "${name}_start"
        } &

        echo "$!" >>"${PID_FILE}"
        # log after storing as logger uses subshell so it changes $!
        log debug "Started plugin: ${name}:${id} with PID: ${!}"
    done

    # start the socket listener
    socat -u UNIX-LISTEN:"$SOCKET",fork - 2>/dev/null |
        while IFS=':' read -r plugin_id event data; do

            log event "${plugin_id}:${event}:${data}"

            case "$event" in
            "update")
                eval "${plugin_id}_data"='${data}'
                plugin_len=$(true_length "${data}")
                eval "${plugin_id}_len=${plugin_len}"
                ;;
            "fg")
                eval "${plugin_id}_fg"='${data}'
                ;;
            "bg")
                eval "${plugin_id}_bg"='${data}'
                ;;
            esac

            if [[ ${plugin_id} == "mouse" ]]; then
                mouse_handler "${event}" "${data}"
            elif [[ ${plugin_id} == "notify" || ${plugin_id} == "osd" ]]; then
                case "${event}" in
                "set")
                    # kill if a previous notification is still writing
                    if kill -0 "${notification_pid}" 2>/dev/null; then
                        kill "${notification_pid}" 2>/dev/null
                    fi

                    # create notification
                    notification="${data}"
                    notification_id="$(date +%s%N | sha256sum | head -c 16)"

                    # show and store pid
                    show_notification "${notification_id}" "${plugin_id}" &
                    notification_pid="$!"
                    ;;
                "clear")
                    # clear notification if the id matches
                    if [[ "${notification_id}" == "${data}" ]]; then
                        notification=""
                        notification_id=""
                        notification_pid=""
                    fi
                    ;;
                esac
            fi

            render
        done
}
