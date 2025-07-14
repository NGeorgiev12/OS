#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
    echo "two args needed"
    exit 1
fi

if [[ ! -f "${1}" ]]; then
    echo "first arg must be file"
    exit 2
fi

if [[ ! "${2}" =~ \.h$ ]]; then
    echo "second arg must be C header file"
    exit 3
fi

inputSize=$(wc -b < "${1}")
((inputSize /= 2))
if [[ "${inputSize}" -gt 524288 ]]; then
    echo "the array is too large"
    exit 4
fi

echo "#include <stdio.h>" >> "${2}"
echo "" >> "${2}"
echo "const uint32_t arrN=${inputSize};" >> "${2}"
echo "" >> "${2}"

echo "const uint16_t arr[] = {" >> "${2}"

xxd "${1}" | cut -c 11-58 | while read -r line; do
    for hex in $line; do
        swapped=$(echo "${hex}" | sed -E 's/^(..)(..)$/\2\1/')
        echo -n "0x${swapped}, " >> "${2}"
    done
done

sed -i '$ s/, $//' "${2}"
echo "};" >> "${2}"
