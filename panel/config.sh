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

######### plugin settings #########

# notify
export notify_fg="${BLUE}"

# song
export song_fg="${BLUE}"
export song_limit=60
export song_format="{{ title }}"
export song_modifiers=(
    "( - )?YouTube Music$/"
)

# battery
export battery_fg="${YELLOW}"
export battery_delay=30

# bluetooth
export bluetooth_fg="${MAGENTA}"
export bluetooth_pipe="/tmp/bluetooth.pipe"

# brightness
export brightness_fg="${GREEN}"
export brightness_delay=30

# cpu
export cpu_fg="${CYAN}"
export cpu_delay=4

# cpu_temp
export cpu_temp_fg="${RED}"
export cpu_temp_delay=10

# date
export date_fg="${YELLOW}"
export date_delay=60

# net_speed
export net_speed_fg="${BLUE}"
export net_speed_delay=5

# ram
export ram_fg="${MAGENTA}"
export ram_delay=5

# time
export time_fg="${CYAN}"
export time_delay=60
