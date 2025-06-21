#!/usr/bin/env bash

net_speed_start() {
    PLUGIN_ID=$1

    while :; do 
        read rx_current tx_current <<< $(awk 'NR>2 { rx+=$2; tx+=$10 } END { print rx, tx }' /proc/net/dev)

        sleep 2

        # store the new network usage
        read rx_new tx_new <<< $(awk 'NR>2 { rx+=$2; tx+=$10 } END { print rx, tx }' /proc/net/dev)

        rx_diff=$(((rx_new - rx_current) / 1024))  
        tx_diff=$(((tx_new - tx_current) / 1024))  

        send update " ${rx_diff}KiB/s  ${tx_diff}KiB/s"

        rx_current=$rx_new
        tx_current=$tx_new

    done
}
