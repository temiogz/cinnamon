#!/bin/bash
if [ $# -ne 2 ]; then
  echo "Usage: $0 <word_to_search> <file_containing_urls>"
  exit 1
fi

echo "SysInfo: "
echo "-------------------"
echo "Operating System: $(uname -s)"
echo "Kernel Version: $(uname -r)"
echo "CPU Information: $(lscpu | grep 'Model name' | awk -F: '{print $2}' | xargs)"
echo "Total Memory: $(free -h | awk '/^Mem/ {print $2}')"
echo "-------------------"

./build/search "$1" "$2"
