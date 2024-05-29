#!/bin/bash
required_packages=("g++" "libcurl4-openssl-dev")

check_package() {
  dpkg -s "$1" &> /dev/null
  if [ $? -ne 0 ]; then
      echo "Error: $1 is not installed."
      exit 1
  fi
}

print_checkmark() {
  printf "\xE2\x9C\x94" 
}

echo "Checking dependencies..."

for package in "${required_packages[@]}"; do
  check_package "$package"
done

# Check if libcurl is linked properly
ldconfig -p | grep libcurl &> /dev/null
if [ $? -ne 0 ]; then
  echo "Error: libcurl is not linked properly."
  exit 1
fi

echo "All dependencies are satisfied $(print_checkmark) ."
