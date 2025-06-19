#!/usr/bin/env bash

cpu_default="󰒡 CPU: 0%"

cpu_start() {
    PLUGIN_ID=$1
    send bg "${GREEN_BG}"
    send fg "${BLACK}"
    while true; do 
        read cpu user nice system idle iowait irq softirq steal guest guest_nice< /proc/stat
        # Calculate CPU usage
        total=$((user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice))
        used=$((total - idle - iowait))

        sleep 1

        read cpu user nice system idle iowait irq softirq steal guest guest_nice< /proc/stat
        # Calculate CPU usage again
        total_new=$((user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice))
        used_new=$((total_new - idle - iowait))

        # Calculate the difference
        diff_total=$((total_new - total))
        diff_used=$((used_new - used))

        # Calculate CPU usage percentage
        cpu_usage=$((100 * diff_used / diff_total))

        send update "󰒡 CPU: ${cpu_usage}%"
    done
}
