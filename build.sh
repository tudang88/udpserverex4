#! /bin/sh

echo 'BUILD PROJECT UDP SERVER'
cmake -S . -B ./build
cmake --build ./build
