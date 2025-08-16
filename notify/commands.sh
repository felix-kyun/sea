_launch() {
    echo "🌊 Starting Sea Notify..."

    CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
    python "$CURRENT_DIR/sea_notify_daemon.py" "sea-notify.sh"
}

_kill() {
    kill "$(ps aux | awk "/[0-9] python.*sea.*notify/ { print \$2 }")"
}
