#! /bin/sh

echo 'BUILD PROJECT UDP SERVER'
rm -rf ./build/*
cmake -S . -B ./build
cmake --build ./build
