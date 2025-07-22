#!/bin/bash
echo "Enabling pin 12 and 13..."
pinctrl 12 a0
pinctrl 13 a0
echo "Compiling..."
g++ -std=c++20 main.cpp Servo_328P.cpp Servo_rpi.cpp -o test_servo
echo "Compiling completed, testing..."
echo "Press Ctrl + C to quit"
sudo ./test_servo
echo "Testing completed, clearing..."
rm test_servo
python test/rpi_328p_set_servos.py
