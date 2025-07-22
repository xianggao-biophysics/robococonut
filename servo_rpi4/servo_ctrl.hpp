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
#pragma once
#include <string>

constexpr int DC_L   =   500000;
constexpr int DC_H   =  2500000;
constexpr int PERIOD = 20000000;
const std::string PATH = "/sys/class/pwm/pwmchip0";

class Servo {
public:
    Servo(int dev);
    ~Servo();
    bool Enable();
    bool Disable();
    double Rotate_to(double fraction);
private:
    static bool WriteToFile(const std::string & path, const std::string & command);
    std::string path_period;
    std::string path_duty_cycle;
    std::string path_enable;
    int dev;
};


