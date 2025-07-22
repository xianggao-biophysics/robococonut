#include <iostream>
#include <fstream>
#include <unistd.h> // for usleep
#include "Servo_rpi.hpp"

std::string path_period = {};
std::string path_duty_cycle {};
std::string path_enable {};

Servo_rpi::Servo_rpi(int dev_in, double min_rescale, double max_rescale): Servo_base(dev_in, min_rescale, max_rescale) {
    static int n = system("pinctrl 12 a0 && pinctrl 13 a0");
    if (n != 0) {
	std::cout << "Error: Failed system call 'pinctrl 12 a0 && pinctrl 13 a0" << std::endl;
	return;
    }
    if(dev != 0 && dev != 1) {
        std::cout << "dev doesn't exist, use dev = 0." << std::endl;
        dev = 0;
    }

    std::string path_export = PATH + "/export";
    if (WriteToFile(path_export, std::to_string(dev))) {
        path_period =     PATH + "/pwm" + std::to_string(dev) + "/period";
        path_duty_cycle = PATH + "/pwm" + std::to_string(dev) + "/duty_cycle";
        path_enable =     PATH + "/pwm" + std::to_string(dev) + "/enable";
	std::cout << "Servo_rpi instance " << dev << " initiated successfully" << std::endl;
    } else {
        std::cout << "Error: Servo_rpi instance " << dev << " initiation failed!" << std::endl;
    }
}

Servo_rpi::~Servo_rpi() {
    std::string path_unexport = PATH + "/unexport";
    WriteToFile(path_unexport, std::to_string(dev));
}

bool Servo_rpi::Enable() {
    if (WriteToFile(path_enable, "1")) {
        WriteToFile(path_period, std::to_string(PERIOD));
	std::cout << "Servo_rpi instance " << dev << " enabled" << std::endl;
        return true;
    } else {
	std::cout << "Error: Servo_rpi instance " << dev << " failed to enable!" << std::endl;
        return false;
    }
}

bool Servo_rpi::Disable() {
    return WriteToFile(path_enable, "0");
}

double Servo_rpi::Rotate_to(double fraction) {
    if (fraction > 1) {
        fraction = 1;
    }
    if (fraction < 0) {
        fraction = 0;
    }
    //std::cout << "fraction before rescale = " << fraction << ", ";
    fraction = rescale(fraction);
    //std::cout << "fraction = " << fraction << std::endl;

    const int target_duty_cycle = static_cast<int>(fraction * (DC_H - DC_L)) + DC_L;
    WriteToFile(path_duty_cycle, std::to_string(target_duty_cycle));
    usleep(1000); // Allow some time for the servo to reach the desired position, adjust as needed
    return fraction;
}

bool Servo_rpi::WriteToFile(const std::string &path, const std::string &val) {
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
