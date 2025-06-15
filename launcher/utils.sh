get_entries() {
    # search all the system paths for .desktop entries
    local desktop_files=(
        /usr/share/applications/
        /usr/local/share/applications/
        ~/.local/share/applications/
        )

    # get all the desktop files locations
    local entries=()
    for folder in "${desktop_files[@]}"; do
        [[ -d "$folder" ]] || continue
        while IFS= read -r file; do
            entries+=("${file}")
        done < <(find $folder -type f -name "*.desktop")
    done 

    printf "%s\n" "${entries[@]}" 
}

process_entries() {
    # process the entries to extract name and exec command into splittable format
    local desktop_files=($(get_entries))
    for file in "${desktop_files[@]}"; do
        (
            name=$(grep -m1 '^Name=' "$file" | cut -d'=' -f2)
            exec=$(grep -m1 '^Exec=' "$file" | cut -d'=' -f2)
            echo "$name|$exec"
        ) &
    done 
}

show_launcher() {
    local selected=$(process_entries | fzf \
        --prompt "Search: " \
        --no-multi \
        --layout reverse \
        --no-scrollbar \
        --border none \
        --delimiter '|' \
        --with-nth '{1}' \
        --accept-nth 2) 

    if [[ -n "$selected" ]]; then
        local exec_command=$(echo "$selected" | sed 's/%[fFuU]//g')
        if [[ -n "$exec_command" ]]; then
            eval "$exec_command &"
        fi
    fi
}
