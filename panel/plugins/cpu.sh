#!/usr/bin/env bash

cpu_default="  0%"
cpu_delay=4

cpu_start() {
    PLUGIN_ID=$1
    while true; do 
        read cpu user nice system idle iowait irq softirq steal guest guest_nice< /proc/stat
        # Calculate CPU usage
        total=$((user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice))
        used=$((total - idle - iowait))

        sleep "${cpu_delay}"

        read cpu user nice system idle iowait irq softirq steal guest guest_nice< /proc/stat
        # Calculate CPU usage again
        total_new=$((user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice))
        used_new=$((total_new - idle - iowait))

        # Calculate the difference
        diff_total=$((total_new - total))
        diff_used=$((used_new - used))

        # Calculate CPU usage percentage
        cpu_usage=$((100 * diff_used / diff_total))

        send update "  ${cpu_usage}%"
    done
}
