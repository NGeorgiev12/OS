#!/bin/bash

if [[ "${#}" -ne 3 ]]; then
    echo "three args needed"
    exit 1
fi

if [[ ! -f "${1}" ]]; then
    echo "first arg must be file"
    exit 2
fi

if [[ ! -f "${2}" || ! -s "${2}" ]]; then
    echo "second arg must be empty file"
    exit 3
fi

if [[ ! -d "${3}" ]]; then
    echo "third arg must be dir"
    exit 4
fi

fileNames=$(mktemp)
find "${3}" -type f -name '*.cfg' > "${fileNames}"

while read fileName; do
    invalidFile=$(mktemp)
    lineCounter=1
    while read line; do
        invalid=$(echo "${line}" | grep -E -v -e "^#(\s[a-z])+$" -e "^{\s[^{}]\s};$")
        if [[ -n "${invalid}" ]]; then
            echo "Line ${lineCounter}:${invalid}" >> "${invalidFile}"
        fi
        ((lineCounter++))
    done< <(cat "${fileName}")
    if [[ ! -s "${invalidFile}" ]]; then
        grep -E "^{\s[^{}]\s};$" "${fileName}" >> "${2}"
    else
        echo "Error in $(basename "${fileName}")"; cat "${invalidFile}"
    fi
    rm "${invalidFile}"
done< "${fileNames}"

username=$(basename "${2}" .cfg)
if ! grep -E -q "${username}" "${1}"; then
    passwd=$(pwgen 16 1)
    passwdHash=$(echo "${passwd}" | md5sum | cut -d ' ' -f 1)
    echo "${username}:${passwdHash}" >> "${1}"
    echo "${username} ${passwd}"
fi
rm "${fileNames}"
