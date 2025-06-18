#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
    echo "two params needed"
    exit 1
fi

if [[ ! -d "${1}" ]]; then
    echo "first arg must be dir"
    exit 2
fi

if [[ ! -d "${2}" || -n $(find "${2}" -mindepth 1) ]]; then
    echo "second arg must be empty dir"
    exit 3
fi

src="${1}"
dest="${2}"

fileNames=$(mktemp)
find "${src}" -type f -name '*.jpg' > "${fileNames}"

mkdir "${dest}/images"
filesInfo=$(mktemp)

while read fileName; do
    simpleFileName=$(basename "${fileName}" .jpg)
    title=$(echo "${simpleFileName}" | sed -E "s/\(.*\)//g"  | tr -s ' ')
    album=$(echo "${simpleFileName}" | sed -E "s/^.*(\(.*\))[^()]*$/\1/" | tr -s ' ')
    if [[ -z "${album}" ]]; then
        album=misc
    fi
    lastMod=$(date -d @"$(stat -c %Y "${fileName}")" +%Y-%m-%d)
    hash=$(sha256sum "${fileName}" | cut -c 1-16)

    cp "${fileName}" "${dest}/images/${hash}.jpg"

    mkdir -p "${dest}/by-date/${lastMod}/by-album/${album}/by-title"
    ln -sr "${dest}/images/${hash}.jpg" "${dest}/by-date/${lastMod}/by-album/${album}/by-title/${title}.jpg"
    mkdir -p "${dest}/by-date/${lastMod}/by-title"
    ln -sr "${dest}/images/${hash}.jpg" "${dest}/by-date/${lastMod}/by-title/${title}.jpg"
    mkdir -p "${dest}/by-album/${album}/by-date/${lastMod}/by-title/"
    ln -sr "${dest}/images/${hash}.jpg" "${dest}/by-album/${album}/by-date/${lastMod}/by-title/${title}.jpg"
    mkdir -p "${dest}/by-album/${album}/by-title"
    ln -sr "${dest}/images/${hash}.jpg" "${dest}/by-album/${album}/by-title/${title}.jpg"
    mkdir -p "${dest}/by-title"
    ln -sr "${dest}/images/${hash}.jpg" "${dest}/by-title/${title}.jpg"
done< "${fileNames}"

rm "${filesInfo}"
rm "${fileNames}"
