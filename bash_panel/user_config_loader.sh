#!/usr/bin/env bash

log config "Loading user configuration"

if [[ -f "${USER_CONFIG}" ]]; then
    source "${USER_CONFIG}"
    log config "Loaded configuration from: ${USER_CONFIG}"
else
    log config "Configuration file not found: ${USER_CONFIG}"
    log config "Using default configuration"
fi
