#!/bin/bash

N=10
if [[ "${1}" == '-n' ]]; then
    if [[ ! "${2}" =~ ^(0|[1-9]+[0-9]*)$ ]]; then
        echo "second arg must be number"
        exit 1
    else
        N="${2}"
    fi
fi

dateFile=$(mktemp)

for arg in "${@}"; do

    if [[ ! -f "${arg}" ]]; then
        continue
    fi

    IDF=$(basename "${arg}" .log)
    tail -n "${N}" "${arg}" | while read line; do
        timestamp=$(echo "${line}" | cut -f 1-2)
        info=$(echo "${line}" | cut -f 3-)
        echo "${timestamp} ${IDF} ${info}" >> "${dateFile}"
    done
done

sort -k 1 "${dateFile}"
rm "${dateFile}"
