#!/usr/bin/env bash

echo "🌊 Starting Sea Notify..."

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
python "$CURRENT_DIR/sea_notify_daemon.py" "sea-notify.sh"  &>/dev/null
