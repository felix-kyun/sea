#!/usr/bin/env bash


include() { 
    source "${CURRENT_DIR}/$1" &>/dev/null; 
}

send () {
    echo -e "${PLUGIN_ID}:${1}:${@:2}" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
}

getid() {
    IFS=':' read name id <<< "$1"

    if [[ -z "${id}" ]]; then 
        echo ${name}
    else
        echo ${id}
    fi
}

getname() {
    IFS=':' read name id <<< "$1"
    echo ${name}
}

process_plugin() {
        data_var="${1}_data"
        fg_var="${1}_fg"
        bg_var="${1}_bg"

        echo "${!bg_var}${!fg_var} ${!data_var} ${RESET}"
}

# stolen snippet
true_length() {
      local input="$1"

      # Remove ANSI color codes
      local clean ansi_removed
      ansi_removed=$(echo -e "$input" | perl -CS -pe 's/\e\[[\d;]*[A-Za-z]//g')

      # Remove other control characters (except tab, newline, carriage return)
      clean=$(echo "$ansi_removed" | tr -dC '\11\12\15\40-\176\200-\377')

      # Use perl Unicode::GCString to count grapheme clusters (visual characters)
      perl -CSDA -MUnicode::GCString -E 'print Unicode::GCString->new($ARGV[0])->length, "\n"' "$clean"
}

transform() {
    declare -n arr=$1
    for i in "${!arr[@]}"; do
        id=$(getid "${arr[$i]}")
        name=$(getname "${arr[$i]}")
        arr[$i]="${name}:${id}"
    done
}
