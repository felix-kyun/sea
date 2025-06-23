#!/usr/bin/env bash

source "$(dirname "${BASH_SOURCE[0]}")/../utils/formatting.sh"

PLUGIN_LEFT=(
    "song"
    )

PLUGIN_RIGHT=(
    "net_speed"
    # "brightness"
    "battery"
    "cpu"
    "ram"
    "cpu_temp"
    )

PLUGIN_CENTER=(
    "time"
    "date"
    )

LOG_FILE="/tmp/sea-panel.log"
LOCK_FILE="/tmp/sea-panel.lock"
SOCKET="/tmp/sea-panel.sock"
PID_FILE="/tmp/sea-panel.pid"

# plugin_colors
battery_fg="${YELLOW}"
brightness_fg="${GREEN}"
cpu_fg="${CYAN}"
ram_fg="${MAGENTA}"
song_fg="${BLUE}"
time_fg="${CYAN}"
date_fg="${YELLOW}"
net_speed_fg="${BLUE}"
cpu_temp_fg="${RED}"
