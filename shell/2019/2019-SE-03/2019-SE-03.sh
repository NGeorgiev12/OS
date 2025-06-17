#!/bin/bash

if [[ "${#}" -ne 1 ]]; then
    echo "one param needed"
    exit 1
fi

if [[ ! -d "${1}" ]]; then
    echo "first param must be dir"
    exit 2
fi

validFileName="^[^_]+_report-[0-9]+\.tgz$"
HASH_FILE="${1}/hashes.txt"

if [[ ! -f "${HASH_FILE}" ]]; then
    touch "${1}/hashes.txt"
fi

fileNames=$(mktemp)
find "${1}" -type f | grep -E "${validFileName}" > "${fileNames}"

while read fileName; do
    extractedDir=$(mktemp -d)
    tar -xzf "${fileName}" -C "${extractedDir}"
    curHash=$(sha256sum "${fileName}" | cut -c 1-64)
    if ! grep -q "${curHash}" "${HASH_FILE}" ; then
        echo "${curHash}  ${fileName}" >> "${HASH_FILE}"

        if [[ -f "${extractedDir}/meow.txt" ]]; then
            name=$(echo "${fileName}" | sed -E '/^([^_]+)_report-[0-9]+$/\1')
            unixTime=$(echo "${fileName}" | sed -E '/^[^_]+_report-([0-9]+)$/\1')
            mkdir -p "${1}/extracted"
            cp "${extractedFile}/meow.txt" "${1}/extracted/${name}_${unixTime}.txt"
        fi
    fi
    rm -rf "${extractedDir}"
done< "${fileNames}"
rm "${fileNames}"
