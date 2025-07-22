#include <iostream>
#include <fstream>
#include <unistd.h> // for usleep
#include "servo_ctrl.hpp"

Servo::Servo(int dev_in): dev(dev_in) {
    if(dev != 0 && dev != 1) {
        std::cout << "dev doesn't exist, use dev = 0." << std::endl;
        dev = 0;
    }

    std::string path_export = PATH + "/export";
    if (WriteToFile(path_export, std::to_string(dev))) {
	path_period =     PATH + "/pwm" + std::to_string(dev) + "/period";
	path_duty_cycle = PATH + "/pwm" + std::to_string(dev) + "/duty_cycle";
	path_enable =     PATH + "/pwm" + std::to_string(dev) + "/enable";
    }
}

Servo::~Servo() {
    std::string path_unexport = PATH + "/unexport";
    WriteToFile(path_unexport, std::to_string(dev));
}

bool Servo::Enable() {
    if (WriteToFile(path_enable, "1")) {
	WriteToFile(path_period, std::to_string(PERIOD));
	return true;
    } else {
	return false;
    }
}

bool Servo::Disable() {
    return WriteToFile(path_enable, "0");
}

double Servo::Rotate_to(double fraction) {
    if (fraction > 1) {
	fraction = 1;
    }
    if (fraction < 0) {
	fraction = 0;
    }

    const int target_duty_cycle = static_cast<int>(fraction * (DC_H - DC_L)) + DC_L;
    WriteToFile(path_duty_cycle, std::to_string(target_duty_cycle));
    return fraction;
}

bool Servo::WriteToFile(const std::string &path, const std::string &val) {
    std::ofstream file(path);
    if(file.is_open()) {
	file << val;
	file.close();
	return true;
    } else {
	std::cerr << "Error: cannot write to file!" << std::endl;
	return false;
    }
}
