#!/usr/bin/env bash

source "$(dirname "${BASH_SOURCE[0]}")/../utils/formatting.sh"

export PLUGIN_LEFT=(
    "bluetooth"
    "song"
)

export PLUGIN_CENTER=(
    "time"
    "date"
)

export PLUGIN_RIGHT=(
    "net_speed"
    "brightness"
    "battery"
    "cpu"
    "ram"
    "cpu_temp"
)

export LOG_FILE="/tmp/sea-panel.log"
export LOCK_FILE="/tmp/sea-panel.lock"
export SOCKET="/tmp/sea-panel.sock"
export PID_FILE="/tmp/sea-panel.pid"

# plugin settings
export bluetooth_fg="${MAGENTA}"
export battery_fg="${YELLOW}"
export brightness_fg="${GREEN}"
export cpu_fg="${CYAN}"
export ram_fg="${MAGENTA}"
export song_fg="${BLUE}"
export time_fg="${CYAN}"
export date_fg="${YELLOW}"
export net_speed_fg="${BLUE}"
export cpu_temp_fg="${RED}"
