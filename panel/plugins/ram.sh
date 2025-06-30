#!/usr/bin/env bash

ram_default="  0%"
ram_delay=${ram_delay:-5}

ram_start() {
    while :; do
        ram=$(free --mebi | awk '/^Mem:/ {print $3/$2 * 100.0}' | awk '{printf "%.0f", $1}')
        send update "  ${ram}%"
        sleep "${ram_delay}"
    done
}
