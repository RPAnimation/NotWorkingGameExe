#!/bin/bash

make clean

make | grep -wE 'gcc' | \
    grep -w '\-o' | \
    jq -nR '[inputs|{directory:".", command:., file: match("-o(.*) ").captures[0].string[1:]}]' \
    > compile_commands.json

CURR_DIR="$(pwd)"
sed -i "s|file\": \"|&${CURR_DIR}/|" compile_commands.json
