 #!/bin/bash
  2
  3 if [[ ${#} != 1 ]]; then
  4     echo "only one param allowed"
  5     exit 1
  6 fi
  7
  8 allFriends=$(mktemp)
  9 escapedDir=$(echo "${1}" | sed -E 's/\//\\\//g')
 10 result=$(mktemp)
 11
 12 find ${1} -maxdepth 3 -mindepth 3 -type d | sed -E "s/^${escapedDir}\/[^/]+\/[^/]+\/([^/]+)$/\1/g" | sort | uniq > "${allFriends}"
 13
 14 while read friend; do
 15      linesCount=$(find ${1} -maxdepth 4 -mindepth 4 -type f | grep -E "^${1}/.+/.+/${friend}/[0-9]{4}(-[0-9]{2}){5}\.txt$" | xargs cat | wc -l)
 16      echo "${friend} ${linesCount}" >> "${result}"
 17 done < <(cat "${allFriends}")
 18
 19 cat ${result} | sort -n -k 2 | tail
 20 rm "${allFriends}" "${result}"