#!/usr/bin/env bash


include() { 
    source "${CURRENT_DIR}/$1" &>/dev/null; 
}

send () {
    data_var="${PLUGIN_ID}_${1}"
    escaped_string=$(echo ${2} | sed 's/"/\\"/g; s/\\/\\\\/g')

    if [[ ! "${!data_var}" == ${2} ]]; then
            echo -e "${PLUGIN_ID}:${1}:${2}" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
            eval "${data_var}=\"${escaped_string}\""
    fi
}
send_noncached () {
    escaped_string=$(echo ${2} | sed 's/"/\\"/g; s/\\/\\\\/g')
    echo -e "${PLUGIN_ID}:${1}:${escaped_string}" | socat - UNIX-CONNECT:"${SOCKET}" 2>/dev/null
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
        local plugin_id="$1"
        data_var="${plugin_id}_data"
        fg_var="${plugin_id}_fg"
        bg_var="${plugin_id}_bg"

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
