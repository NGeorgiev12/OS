#!/bin/bash

if [[ "${#}" -eq 0 ]]; then
    echo "file must have args"
    exit 1
fi

fqdnRegex="([a-z0-9]\.)+"
ttlRegex="([0-9]+[[:space:]])?"
classRegex="IN"
typeRegex="SOA"
numRegex="[0-9]{10}[[:space:]]([0-9]+[[:space:]]){4}"
lineRegex="${fqdnRegex}[[:space:]]${ttlRegex}${classRegex}[[:space:]]${typeRegex}[[:space:]](\(|${numRegex})"

for fileName in "${@}"; do

    if [[ ! -f "${fileName}" ]]; then
        echo "${fileName} must be file"
        continue
    fi

    line=$(grep - E "${lineRegex}" "${fileName}")

    if [[ -z "${line}" ]]; then
        echo "Invalid SOA info of file ${fileName}"
        continue
    fi

    lastChar=$(echo "${line}" | grep -o -E "\($")
    serial=""
    if [[ "${lastChar}" == '(' ]]; then
        serial=$(sed -E "s/^[[:space:]]([0-9]{10})[[:space:]];[[:space:]]serial[[:space:]]$/\1/" "${fileName}")
    else
        serial=$(echo "${line}" | grep -E -o "[0-9]{10}")
    fi

    date=$(echo "${serial}" | cut -c 1-8)
    counter=$(echo "${serial}" | cut -c 9-10)

    today=$(date '+%Y%m%d')
    newSerial=""
    if [[ "${today}" == "${date}" ]]; then
        ((counter++))
        newSerial="${date}${counter}"
    else
        newSerial="${today}00"
    fi

    sed -E -i "s/${serial}/${newSerial}" "${fileName}"

done
