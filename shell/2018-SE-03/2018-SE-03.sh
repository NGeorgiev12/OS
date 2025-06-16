#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
    echo "two params needed"
    exit 1
fi

if [[ ! -f "${1}" ]]; then
    echo "first param must be file"
    exit 2
fi

if [[ ! -f "${2}" || -s  "${2}" ]]; then
    echo "second param must be empty file"
    exit 3
fi

invalid=$(grep -E -v '^[0-9]+(,[^,]*)*$' "${1}")
uniqueLines=$(mktemp)

if [[ -n ${invalid} ]]; then
    echo "first file doesn't meet the criterias"
    exit 4
fi

cut -d ',' -f 2- "${1}" | sort | uniq > "${uniqueLines}"

while read line; do
    tempLineFile=$(mktemp)
    grep -F ",${line}" "${1}" > "${tempLineFile}"
    linesCount=$(cat "${tempLineFile}" | wc -l)
    if [[ "${linesCount}" -gt 1 ]]; then
        minIdtemp=$(mktemp)
        cut -d ',' -f 1 "${tempLineFile}" > "${minIdtemp}"
        minId=1000000
        while read num; do
            if [[ "${minId}" -lt "${num}" ]]; then
                minId="${num}"
            fi
        done< <(cat "${minIdTemp}")
        echo "${minId},${line}" >> "${2}"
        rm "${minIdtemp}"
    else
        echo "${tempLineFile}" >> "${2}"
    fi
    rm "${tempLineFile}"
done< <(cat "${uniqueLines}")

rm "${uniqueLines}"
