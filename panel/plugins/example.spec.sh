#!/usr/bin/env bash

# default value before the plugin send update
example_default="example"

# runs once when the plugin is loaded
example_onload() {
    log info "Example plugin loaded"
}

# runs when the plugin is started
# this function must be defined
example_start() {

    # example controls
    send bg "${BLUE_BG}"
    send fg "${WHITE}"
    send update "󰒡 Example Plugin"

    while :; do
        # Simulate some work
        sleep 2
        send update "Example Plugin is running"
    done
}

# run when the bar is closed
example_unload() {
    log info "Example plugin unloaded"
}
