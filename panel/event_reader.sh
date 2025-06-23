#!/usr/bin/env bash

parse_sgr_mouse() {
    local seq="$1"
    local btn x y act

      IFS=';' read -r btn x rest <<< "$seq"
      y="${rest%[mM]*}"      # Remove trailing M or m for y
      act="${rest: -1}"      # Last character is M (press/scroll) or m (release)

      local event_type="unknown"
      case "$btn" in
        0|1|2) event_type="press" ;;
        3)     event_type="release" ;;
        64)    event_type="scroll-up" ;;
        65)    event_type="scroll-down" ;;
        *)     event_type="unknown" ;;
      esac

    send "panel:${event_type}:${x};${y}"
}

event_reader() {
    while :; do
        while IFS= read -rsn1 -t 1 ch; do
            if [[ $ch == $'\033' ]]; then
                IFS= read -rsn1 ch2
                if [[ $ch2 == "[" ]]; then
                    IFS= read -rsn1 ch3
                    if [[ $ch3 == "<" ]]; then
                        seq=""
                        while IFS= read -rsn1 ch4; do
                            seq+="$ch4"
                            [[ $ch4 == "m" || $ch4 == "M" ]] && break
                        done
                        parse_sgr_mouse "${seq}"
                    fi
                fi
            fi
        done
    done
}
