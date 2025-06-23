#!/usr/bin/env bash

mouse_handler() {
    local event="$1"
    IFS=';' read x y <<< "$2"

    log mouse "${event} at (${x}, ${y})"
}
