#!/bin/bash

if echo "${*}" | grep -E -v -q '-jar'; then
    echo "invalid java command" >& 2
    exit 1
fi

flag="options"

options=""
filename=""
args=""

for arg in "${@}"; do

    if [[ "${flag}" == "options" ]]; then

        if [[ "${arg}" == "-jar" ]]; then

            flag="DOptions"
            continue
        fi

        if [[ ! "${arg}" =~ ^-D ]]; then

            options=$(echo ${options} ${arg})
        fi
    fi

    if [[ "${flag}" == "DOptions" ]]; then

        if [[ "${arg}" =~ .*.jar$ ]]; then

            flag="args"
            filename=${arg}
            continue
        fi

        if [[ "${arg}" =~ ^- ]]; then

            options=$(echo ${options} ${arg})
        fi
    fi

    if [[ "${flag}" == "args" ]]; then

        args=$(echo "${args}" "${arg}")
    fi

done

options=$(echo "${options}" | sed -E 's/^ //')
filename=$(echo "${filename}" | sed -E 's/^ //')
args=$(echo "${args}" | sed -E 's/^ //')

java ${options} -jar ${filename} ${args}
