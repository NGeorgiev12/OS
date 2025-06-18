#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
    echo "two args needed"
    exit 1
fi

if [[ ! -d "${1}" || ! -d  "${2}" ]]; then
    echo "the two params must be dirs"
    exit 2
fi
repo="${1}"
package="${2}"
dbFile="${repo}/db"

packageName=$(basename "${package}")
packageVersion=$(cat "${package}"/version)

grep -E "^${packageName}" "${dbFile}" | sed -E 's/^.*-(.*\s.*)$/\1/' > "${packageVersionsAndCheckSums}"

if grep -E -q "${packageVersion}" "${packageVersionsAndCheckSums}"; then
    toDelete=$(grep -E "${packageVersion}" "${packageVersionsAndCheckSums}" | cut -d ' ' -f 2)
    rm "${toDelete}"
fi

tarFile=$(mktemp)
tar -czf "${tarFile}" -C "${package}/tree" .
hash=$(sha256sum "${tarFile}")
cp "${tarFile}" "${repo}/packages/${hash}.tar.xz"

grep -v -E "^${packageName}-${packageVersion} " "${dbFile}" > "${dbFile}.tmp"
echo "${packageName}-${packageVersion} ${hash}" >> "${dbFile}.tmp"
sort "${dbFile}.tmp" > "$dbFile"

rm "${dbFile}.tmp"
rm "${tarFile}"
