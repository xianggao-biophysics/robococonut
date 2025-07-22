#!/bin/bash
echo "compiling"
g++ -o client rpi_test_webcam.cpp `pkg-config --cflags --libs opencv4`
echo "running test"
./client
rm client
