#pragma once

class Servo_base {
public:
    Servo_base(int dev, double min_rescale = 0.0, double max_rescale = 1.0) : dev(dev), min_rescale(min_rescale), max_rescale(max_rescale) {}; 
    ~Servo_base() {};
    virtual bool Enable() { return true; };
    virtual bool Disable() { return true; };
    virtual double Rotate_to(double fraction) = 0;  // Rotate the servo to a specified position as a fraction of the full range (0.0 to 1.0)
protected:
    int dev = 0;
    double min_rescale = 0.0; // default range is 0.0 - 1.0, rescale it so the fraction's range is still 0 - 1, but the actual value rotate to is rescaled.
    double max_rescale = 1.0;
    double rescale(double v) { return min_rescale + (max_rescale - min_rescale) * v; };
};


