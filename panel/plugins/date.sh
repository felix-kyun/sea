#!/usr/bin/env bash

date_default="󰃰 00/00/00"
date_delay="${date_delay:-60}"

date_start() {
    while :; do
        current_date=$(date +"%a %b %d %Y")
        send update "󰃰 ${current_date}"
        sleep "${date_delay}"
    done
}
