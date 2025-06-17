#!/bin/bash

integer='-?(0|[1-9]+[0-9]*)'
decimal='-?(0|[1-9]+[0-9]*.[0-9]*)'
numFile=$(mktemp)

while read line; do
    if [[ "${line}" =~ $decimal ]]; then
        echo "there is decimal number on the line"
        exit 1
    fi

    if [[ ! "${line}" =~ $integer ]]; then
        echo "there is not integer number on the line"
        exit 2
    fi

    echo "${line}" | grep -E -o "${integer}" >> "${numFile}"

done

numFileExtended=$(mktemp)

while read num; do
    sumOfDigits=$(sed -E -e 's/-//g' -e 's/./&+/g' -e 's/\+$//' "${num}" | bc)
    echo "${num} ${sumOfDigits}" >> "${numFileExtended}"
done< <(sort -n "${numFile}" | uniq)

sort -k 2,2nr -k 1,1n "${numFileExtended}" | head -n 1 | cut -d ' ' -f 1

rm "${numFileExtended}"
rm "${numFile}"
