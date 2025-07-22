#pragma once
#include <string>
#include "Servo_base.hpp" // Include the base class for Servo
/*
To enable both PWMs (GPIO pin 12 and 13):

```bash
sudo vim /boot/firmware/config.txt
```

add this

```bash
dtoverlay=pwm-2chan,func=gpio,pin=12,pin2=13
```

check if both are activate using:

```bash
pinctrl 12
pinctrl 13
```

if they are not pwm0_0 and pwm0_1, do this command:

```bash
pinctrl 12 a0
pinctrl 13 a0
```
*/
constexpr int DC_L   =   500000;
constexpr int DC_H   =  2500000;
constexpr int PERIOD = 20000000;
const std::string PATH = "/sys/class/pwm/pwmchip0";

class Servo_rpi : public Servo_base {
    // This class controls a servo motor on a Raspberry Pi using the sysfs interface for PWM
    // dev: 0 or 1 for pwm0 or pwm1 respectively
    // path_period, path_duty_cycle, and path_enable are used to access the sysfs interface for PWM control
    // DC_L and DC_H are the duty cycle limits for the servo in microseconds
    // PERIOD is the total period of the PWM signal in nanoseconds (20ms)
    // The class provides methods to enable/disable the servo and rotate it to a specified position as a fraction of the full range (0.0 to 1.0)
public:
    Servo_rpi(int dev, double min_rescale = 0.0, double max_rescale = 1.0);
    ~Servo_rpi();
    bool Enable() override; // Enable the PWM output for the servo, setting the period to 20ms
    bool Disable() override; // Disable the PWM output for the servo, effectively turning it off
    double Rotate_to(double fraction) override;
private:
    static bool WriteToFile(const std::string & path, const std::string & command);
    std::string path_period;
    std::string path_duty_cycle;
    std::string path_enable;
};


