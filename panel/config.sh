#!/usr/bin/env bash

source "$(dirname "${BASH_SOURCE[0]}")/../utils/formatting.sh"
PLUGINS=(
    "battery"
    "brightness"
    "cpu"
    "ram"
    "song"
    "time"
    "date"
    "net_speed"
    "cpu_temp"
    )

PLUGIN_LEFT=(
    "song:song"
    )

PLUGIN_RIGHT=(
    "net_speed:net_speed"
    "brightness:brightness"
    "battery:battery"
    "cpu:cpu"
    "ram:ram"
    "cpu_temp:cpu_temp"
    )

PLUGIN_CENTER=(
    "time:time"
    "date:date"
    )

LOG_FILE="/tmp/sea-panel.log"
LOCK_FILE="/tmp/sea-panel.lock"
SOCKET="/tmp/sea-panel.sock"

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
