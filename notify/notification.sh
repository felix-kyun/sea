#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source "${CURRENT_DIR}/../utils/formatting.sh"

app_name=$1
summary=$2
body=$3
timeout=$4
icon=$5

# 30x3
printf '\e[?1000h'
echo -e "${HIDE_CURSOR}${BOLD}${CYAN}${app_name}${WHITE} ${summary}${RESET}"
echo -e "${body}${RESET}"

read -rsn6 
printf '\e[?1000l'
