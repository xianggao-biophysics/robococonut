#!/bin/bash
g++ servo_ctrl.cpp servo_test.cpp -o servo_test
sudo ./servo_test
rm servo_test
