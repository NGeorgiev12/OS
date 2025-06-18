#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
    echo "two params needed"
    exit 1
fi

if [[ ! -f "${1}" || ! "${1}" =~ 'bar.csv' ]]; then
    echo "first param must be file named bar.csv"
    exit 2
fi

if [[ -s "${1}" ]]; then
    echo "file must be empty"
    exit 3
fi

if [[ ! -d "${2}" ]]; then
    echo "second param must be dir"
    exit 4
fi

fileNames=$(mktemp)
find "${2}" -type f > "${fileNames}"

echo "hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key" >> "${1}"

while read file; do
    hostname=$(basename "${file}" .log)

    line=$(sed -E -e 's/^.*(a|an)\s(.*)\slicense\.$/\2/g' -e 's/^.*:\s(.*)$/\1/g' "${file}" | tr '\n' ',' | sed -E 's/,$//')
    echo "${hostname},${line}" >> "${1}"
done< "${fileNames}"
rm "${fileNames}"
