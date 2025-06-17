#!/bin/bash

integer='-?(0|[1-9]+[0-9]*)'
decimal='-?(0|[1-9]+[0-9]*).[0-9]+'
numFile=$(mktemp)

while read line; do

    if [[ "${line}" =~ $decimal ]]; then
        echo "there is decimal on the line"
        continue
    fi

    if [[ ! "${line}" =~ $integer ]]; then
        echo "there is not any integer on the line"
        continue
    fi

    echo "${line}" | grep -E -o "$integer" >> "${numFile}"

done

maxAbs=0
while read num; do
    abs=${num#-}
    if [[ "${abs}" -gt "${maxAbs}" ]]; then
        maxAbs="${abs}"
    fi
done< <(cat ${numFile} | sort -n | uniq)

while read num; do
    abs=${num#-}
    if [[ "${abs}" -eq "${maxAbs}" ]]; then
        echo "${num}"
    fi
done< <(cat "${numFile}")
rm "${numFile}"
