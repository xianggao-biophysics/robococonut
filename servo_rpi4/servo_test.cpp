
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <array>
#include <thread>
#include <climits>
#include <atomic>
#include <unistd.h> // for usleep
#include "servo_ctrl.hpp"

namespace {
const int number_of_servos = 2;
constexpr double start[] = {0.45,0.0}; // 0.43 is the min for servo 0
constexpr double end[] = {0.99, 1.0}; // 0.92 is the max for servo 0
constexpr int steps = 100;
constexpr double incre[] = {(end[0] - start[0]) / steps, (end[1] - start[1]) / steps};
constexpr double idle[] = {start[0], (start[1] + end[1]) / 2.0};
constexpr double midpoint[] = {(start[0] + end[0]) / 2.0, (start[1] + end[1]) / 2.0};
constexpr double radius[] = {(end[0] - start[0]) / 3.0, (end[1] - start[1]) / 3.0};
constexpr double total_time = 5.0; //second
constexpr double step_time = total_time * 1000.0 * 1000.0 / steps;
constexpr double step_angle = 2 * 3.1415926 / steps;
}

/*
int main() {
    Servo s(0);
    s.Enable();
    for (int i = 0; i < steps; ++i) {
	s.Rotate_to(start[0] + incre[0] * i);
	usleep(step_time);
    }
    s.Disable();
    return 0;
}
*/


void helper_look_around(std::array<Servo, 2>& servos) {
    for (int i = 0; i < steps; ++i) {
        servos[0].Rotate_to(start[0] + incre[0] * i);
        usleep(step_time);
    }
    for (int i = 0; i < steps; ++i) {
        servos[1].Rotate_to(start[1] + incre[1] * i);
        usleep(step_time);
    }
    for (int i = steps - 1; i >= 0; --i) {
        servos[0].Rotate_to(start[0] + incre[0] * i);
        usleep(step_time);
    }
    for (int i = steps - 1; i >= 0; --i) {
        servos[1].Rotate_to(start[1] + incre[1] * i);
        usleep(step_time);
    }
}

void helper_look_around_2(std::array<Servo, 2>& s) {
    for (int i = 0; i < steps; ++i) {
	s[0].Rotate_to(midpoint[0] + 0.5 * (end[0] - start[0]) * sin(step_angle * i));
	s[1].Rotate_to(midpoint[1] + 0.5 * (end[1] - start[1]) * cos(step_angle * i));
	usleep(step_time);
    }
}

std::atomic<bool> running(true);
void spinLoop() {
    std::array<Servo, 2> servos = {Servo(0), Servo(1)};
    
    for (auto& s: servos) {
	s.Enable();
    }
    int starting_steps = 1000;
    for (int i = 0; i <= starting_steps; ++i) {
	double y1 = (i * (0.5 * (start[0] + end[0])) + (starting_steps - i) * idle[0]) / starting_steps;
	double x1 = (i * (0.5 * (start[1] + end[1]) + radius[1]) + (starting_steps - i) * idle[1]) / starting_steps;
        servos[0].Rotate_to(y1);
	servos[1].Rotate_to(x1);
	usleep(200 + i);
    }	

    int i = 0;
    double x = 0, y = 0;
    while(running.load() && i < INT_MAX) {
	y = (0.5 * (start[0] + end[0]) + radius[0] * sin(step_angle * i));
	x = (0.5 * (start[1] + end[1]) + radius[1] * cos(step_angle * i));
	servos[0].Rotate_to(y);
	servos[1].Rotate_to(x);
	usleep(step_time);
	++i;
    }
    int restore_steps = 1000;
    for (int i = 0; i <= restore_steps; ++i) {
	double y1 = (i * idle[0] + (restore_steps - i) * y) / restore_steps;
	double x1 = (i * idle[1] + (restore_steps - i) * x) / restore_steps;
	servos[0].Rotate_to(y1);
	servos[1].Rotate_to(x1);
	usleep(200 + i);
    }

    for (auto& s: servos) {
	s.Disable();
    }
}

void waitForInput() {
    std::cout << "Press any key to stop the spin loop...\n";
    std::cin.get();
    running.store(false);
}

int main() {
    std::thread spinner(spinLoop);
    std::thread inputListener(waitForInput);

    spinner.join();
    inputListener.join();

    std::cout << "Test is done!\n";
    return 0;
}
