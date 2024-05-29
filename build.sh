#!/bin/bash
mkdir -p build
g++ -std=c++11 -o build/search search.cpp -lcurl