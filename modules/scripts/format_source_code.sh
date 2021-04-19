#!/bin/bash

set -e

CLANG_FORMAT=$(which clang-format) || (echo "Please install 'clang-format' tool first"; exit 1)

version=$("${CLANG_FORMAT}" --version | sed -n "s/.*\ \([0-9]*\)\.[0-9]*\.[0-9]*.*/\1/p")
if [[ "${version}" -lt "8" ]]; then
  echo "clang-format's version must be at least 8.0.0"
  exit 1
fi

CURRENT_PATH=$(pwd)
SCRIPTS_PATH=$(dirname "$0")

echo "CURRENT_PATH=${CURRENT_PATH}"
echo "SCRIPTS_PATH=${SCRIPTS_PATH}"

# print usage message
function usage()
{
  echo "Format the specified source files to conform the code style."
  echo "Usage:"
  echo "bash $0 [-a] [-c] [-l] [-h]"
  echo "e.g. $0 -c"
  echo ""
  echo "Options:"
  echo "    -a format of all files"
  echo "    -c format of the files changed compared to last commit, default case"
  echo "    -l format of the files changed in last commit"
  echo "    -h Print usage"
}

# check and set options
function checkopts()
{
  # init variable
  mode="changed"    # default format changed files

  # Process the options
  while getopts 'aclh' opt
  do
    case "${opt}" in
      a)
        mode="all"
        ;;
      c)
        mode="changed"
        ;;
      l)
        mode="lastcommit"
        ;;
      h)
        usage
        exit 0
        ;;
      *)
        echo "Unknown option ${opt}!"
        usage
        exit 1
    esac
  done
}

# init variable
# check options
checkopts "$@"

# switch to project root path, which contains clang-format config file '.clang-format'
cd "${SCRIPTS_PATH}/../.." || exit 1

FMT_FILE_LIST='__format_files_list__'

if [[ "X${mode}" == "Xall" ]]; then
  find src/{pipeline,parse,utils} -type f -name "*" | grep "\.h$\|\.cc$\|\.c$" > "${FMT_FILE_LIST}" || true
elif [[ "X${mode}" == "Xchanged" ]]; then
  git diff --name-only | grep "src/pipeline\|src/parse\|src/utils\|tests" | grep "\.h$\|\.cc$\|\.c$" > "${FMT_FILE_LIST}" || true
else  # "X${mode}" == "Xlastcommit"
  git diff --name-only HEAD~ HEAD | grep "src/pipeline\|src/parse\|src/utils\|tests" | grep "\.h$\|\.cc$\|\.c$" > "${FMT_FILE_LIST}" || true
fi

while read line; do
  if [ -f "${line}" ]; then
    ${CLANG_FORMAT} -i "${line}"
  fi
done < "${FMT_FILE_LIST}"

rm "${FMT_FILE_LIST}"
cd "${CURRENT_PATH}" || exit 1

echo "Specified cpp source files have been format successfully."
