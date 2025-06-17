  1 #!/bin/bash
  2
  3 if [[ "${#}" -ne 2 ]]; then
  4     echo "two args needed"
  5     exit 1
  6 fi
  7
  8 if [[ !  -f "${1}" ]]; then
  9     echo "first arg must be file"
 10     exit 2
 11 fi
 12
 13 if [[ ! -d "${2}" ]]; then
 14     echo "second arg must be dir"
 15     exit 3
 16 fi
 17
 18 if [[ -n $(find "${2}" -mindepth 1) ]]; then
 19     echo "the dir must be empty"
 20     exit 4
 21 fi
 22
 23 invalid=$(grep -E -v '^[a-zA-Z-]+[[:space:]][a-zA-Z-]+.*:[^\n]*$' "${1}")
 24
 25 if [[ -n ${invalid} ]]; then
 26     echo "file doesn't meet the criterias"
 27     exit 5
 28 fi
 29
 30 allUsers=$(mktemp)
 31 counter=1
 32
 33 touch "${2}/dict.txt"
 34 grep -E -o '[a-zA-Z-]+[[:space:]][a-zA-Z-]+' "${1}" | sort | uniq > "${allUsers}"
 35
 36 while read name; do
 37     echo "${name};${counter}" >> "${2}/dict.txt"
 38     touch "${2}/${counter}.txt"
 39     grep -E "^${name}" "${1}" > "${2}/${counter}.txt"
 40     ((counter++))
 41 done< <(cat "${allUsers}")
 42 rm "${allUsers}"
