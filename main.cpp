#include <iostream>
#include <vector>
#include "Servo_rpi.hpp"
#include "Servo_328P.hpp"

void test(std::vector<Servo_base*> servos){
    int i = 0;
    bool dir = true; // true = ++i;
    while (true) {	
        for(auto servo : servos) { // Iterate over each servo in the array
            double fraction = i / 100.0; // Calculate the fraction for rotation (0.0 to 1.0)
            if (servo) {
                servo->Rotate_to(fraction); // Rotate the servo to the specified fraction
                // std::cout << "Rotated to " << fraction * 100 << "%\n";
            } else {
                std::cerr << "Servo pointer is null\n";
            }
        }
        usleep(20000); // Sleep for 50ms to allow the servo to reach the position before moving to the next one
        if (dir) {
	    ++i;
	} else {
	    --i;
	}
	if (i == 0 || i == 100) {
	    dir = !dir;
	}
    }
}

int main() {
    // 328p 0 center 480, 1 center 440
    Servo_328P right_wheel(0, 0.28, 0.68); // Create an instance for the right wheel servo on device 1
    Servo_328P left_wheel(1, 0.64, 0.24); // Create an instance for the left wheel servo on device 0
    Servo_rpi cam_vertical(0, 0.55, 1.0); // Create an instance for the camera vertical servo on PWM device 0 (Raspberry Pi)
    Servo_rpi cam_horizontal(1); // Create an instance for the camera horizontal servo on PWM device 1 (Raspberry Pi)
    std::vector<Servo_base*> all_servos = { &left_wheel, &right_wheel, &cam_vertical, &cam_horizontal };
    for (auto s : all_servos) {
        s->Enable();
    }
    // test these servos
    test({ &left_wheel, &right_wheel, &cam_vertical, &cam_horizontal });
    //test({ &cam_vertical, &cam_horizontal });
    //test({ &cam_vertical });
    return 0;
}
