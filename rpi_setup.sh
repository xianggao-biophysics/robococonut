#!/bin/bash
sudo apt update
sudo apt upgrade -y

# Install dependencies for Arduino IDE and OpenCV
sudo apt install build-essential cmake arduino libopencv-dev python3-opencv fswebcam vim -y

# Add user to dialout group for serial port access
# This allows the user to access serial devices without sudo
sudo usermod -aG dialout $USER

# Enable GPIO pins 12 and 13 for PWM
sudo bash -c 'echo "dtoverlay=pwm-2chan,func=gpio,pin=12,pin2=13" >> /boot/firmware/config.txt'
sudo pinctrl 12 a0
sudo pinctrl 13 a0

PWM_DIR="/sys/class/pwm/pwmchip0"

# Export both channels
echo 0 > "$PWM_DIR/export"
echo 1 > "$PWM_DIR/export"

# Set period and duty cycle
echo 20000000 > "$PWM_DIR/pwm0/period"
echo 20000000 > "$PWM_DIR/pwm1/period"
echo   500000 > "$PWM_DIR/pwm0/duty_cycle"
echo  2500000 > "$PWM_DIR/pwm1/duty_cycle"

# Enable channels
echo 1 > "$PWM_DIR/pwm0/enable"
echo 1 > "$PWM_DIR/pwm1/enable"

echo "PWM signals enabled on both channels. You can now control servos using GPIO pins 12 and 13."

# Install Arduino CLI
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

# Install Docker
curl -fsSL https://download.docker.com/linux/debian/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/debian \
  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt-get update
sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin -y

# Verify Docker installation. Check if Docker is running
sudo systemctl enable docker
sudo systemctl start docker
sudo systemctl status docker
sudo docker --version

# Add user to docker group
# This allows running Docker commands without sudo
sudo usermod -aG docker $USER

# The command below won't work because these images are linux/amd64, not linux/arm64/v8
# ROS2 installation
# https://github.com/osrf/docker_images
# sudo docker pull osrf/ros:jazzy-desktop
# docker run -it --rm -v /absolute/path/to/src:/src ros:jazzy-desktop

# instead, I will follow this guide:
# https://docs.ros.org/en/jazzy/How-To-Guides/Installing-on-Raspberry-Pi.html
docker pull ros:jazzy-ros-base
docker run -it --rm ros:jazzy-ros-base

# Or I can build image from osrf/ros github file:
# https://github.com/osrf/docker_images/tree/master/ros/jazzy/ubuntu/noble/ros-base/Dockerfile
# Inside the directory, run `docker build -t ros:ros_docker .`

sudo reboot
