#!/usr/bin/env bash

panel_pid=$(ps aux | grep bash | grep "kitty.*panel/main.sh" | awk '{ print $2 }')
kill "${panel_pid}" &>/dev/null
