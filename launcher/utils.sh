#!/usr/bin/env bash

get_entries() {
    # search paths for .desktop entries
    local desktop_files=(
        # system wide applications
        /usr/share/applications/
        /usr/local/share/applications/
        /var/lib/flatpak/exports/share/applications/

        # user specific applications
        ~/.local/share/applications/

        # for flatpak apps
        ~/.local/share/flatpak/exports/share/applications/
        )

    # get all the desktop files locations
    local entries=()
    for folder in "${desktop_files[@]}"; do
        [[ -d "$folder" ]] || continue
        while IFS= read -r file; do
            entries+=("${file}")
        done < <(find $folder -type f -o -type l -name "*.desktop")
    done 

    printf "%s\n" "${entries[@]}" 
}

process_entries() {
    # process the entries to extract name and exec command into splittable format
    local desktop_files=($(get_entries))
    for file in "${desktop_files[@]}"; do
        (
            # skip entries that are marked as NoDisplay=true
            nodisplay=$(grep -m1 '^NoDisplay=' "$file" | cut -d'=' -f2)
            [[ "$nodisplay" == "true" ]] && return

            name=$(grep -m1 '^Name=' "$file" | cut -d'=' -f2)
            exec=$(grep -m1 '^Exec=' "$file" | sed 's/^Exec=//')

            # skip entries without a name or exec command
            [[ -z "$name" || -z "$exec" ]] && return

            echo "$name|$exec"
        ) &
    done 
}

# double fork 
# setsid(on its own), disown, and nohup didnt work
launch_detached() {
  (
    setsid bash -c "$1" < /dev/null > /dev/null 2>&1 &
  ) &
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

    local exec_command=$(echo "$selected" | sed 's/%[fFuU]//g')
    launch_detached "$exec_command"
    sleep 0.01 # give it a moment to launch
}
