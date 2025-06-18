#!/bin/bash

if [[ "${#}" -ne 3 ]]; then
    echo "three args needed"
    exit 1
fi

if [[ ! -f "${1}" ]]; then
    echo "first arg must be file"
    exit 2
fi

valid="[a-zA-Z0-9_]+"

if [[ ! "${2}" =~ ${valid} ]]; then
    echo "not allowed symbols in the key"
    exit 3
fi

if [[ ! "${3}" =~ ${valid} ]]; then
    echo "not allowed symbols in the value"
    exit 4
fi

if ! grep -q -E "(^(\s)*${valid}(\s)*=(\s)*${valid}(\s[#.*]*)*$)|(^#.*$)|(^(\s)*$)" "${1}"; then
    echo "file is not valid"
    exit 5
fi

lineCounter=1
isModified=0
while read line; do
    if echo "${line}" | grep -E -q "${2}"; then
        sed -i -E "s/^((\s)*${2}.*)$/#\1 # edited at $(date) by $(whoami)/" "${1}"
        sed -i "${lineCounter}a ${2} = ${3} # added at $(date) by $(whoami)" "${1}"
        isModified=1
        break
    fi
    ((lineCounter++))
done< "${1}"

if [[ "${isModified}" -eq 0 ]]; then
    sed -i "$a ${2} = ${3} # added at $(date) by $(whoami)" "${1}"
fi
