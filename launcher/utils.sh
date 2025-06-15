get_desktop_files() {
    # search all the system paths for .desktop entries
    local desktop_files=(
        /usr/share/applications/
        /usr/local/share/applications/
        ~/.local/share/applications/
        )

    local entries=()
    for folder in "${desktop_files[@]}"; do
        [[ -d "$folder" ]] || continue
        while IFS= read -r file; do
            entries+=("${file}")
        done < <(find $folder -type f -name "*.desktop")
    done 

    printf "%s\n" "${entries[@]}" | sort -u 
}

create_map(){
    local desktop_files=($(get_desktop_files))
    local map=()
    for file in "${desktop_files[@]}"; do
        local name=$(grep -m1 '^Name=' "$file" | cut -d'=' -f2)
        local exec=$(grep -m1 '^Exec=' "$file" | cut -d'=' -f2)
        map+=("$name|$exec")
    done
    printf "%s\n" "${map[@]}"
}

show_launcher() {
    local selected=$(create_map | fzf \
        --prompt "Search: " \
        --no-multi \
        --layout reverse \
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
