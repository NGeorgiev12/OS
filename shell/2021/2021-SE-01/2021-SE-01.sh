#!/bin/bash

if [[ "${#}" -ne 0 ]]; then
    echo "no args needed"
    exit 1
fi

if ! echo "$(whoami)" | grep -E -q '^oracle$|^grid$'; then
    echo "only users oracle and grid can execute the script"
    exit 2
fi

temp=$(mktemp)
diagDest="/u01/app/$(whoami)"

${ORACLE_HOME}/bin/adrci -exec "show homes" | tail -n +2 > "${temp}"

if [[ -s "${temp}" ]]; then
    while read line; do
        absPath="${diagDest}/${line}"
        du -sm "${absPath}"
    done< "${temp}"
fi
rm "${temp}"
