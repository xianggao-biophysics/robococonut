# A robot car that can detect and follow a moving object (i.e. me)

My first project, a robot car

![robococonut1](https://github.com/xianggao-biophysics/robococonut/blob/main/robococonut.jpeg)


+ Raspberry Pi 4: Ros running on a docker, image acquisition and 2 servo motors
+ Windows host: WSL2 with ROS
+ ATmega 328P ( will be replaced by ESP32): control 2 servo moters and a DC motor control board that control 2 DC motors.
+ wifi connection (TCP) for rpi4-host communication, and Serial for rpi-328P communication.
