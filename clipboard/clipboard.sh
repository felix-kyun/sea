#!/usr/bin/env bash
CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source "${CURRENT_DIR}/../utils/fzf_theme_override.sh"

cliphist-fzf-copy() {
  local sel
  sel=$(cliphist list | fzf -d $'\t' --with-nth 2)
  [ -n "$sel" ] && cliphist decode "$sel" | wl-copy 2>/dev/null
}
