#!/usr/bin/env bash

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$CURRENT_DIR/../modules" || exit 1

{
    echo "// This file is auto-generated. Do not edit manually."
    echo "// created by create_modules_list.sh"
    echo "#pragma once"
    echo "const char* inbuilt_modules[] = {"

    find . -type f \
        | sed 's/\.\///; s/\.c$//; s/\(.*\)/"\1"/' \
        | awk -f "../scripts/find_modules.awk" 

    echo "};"
} > "../include/modules/inbuilt_modules.h"
