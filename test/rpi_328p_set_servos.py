import time
import serial
import struct

# Open serial connection
ino = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
time.sleep(2)  # Wait for Arduino to initialize

def send(pwm_id, value):
    if pwm_id < 0 or pwm_id > 1:
        print("Invalid PWM ID")
        return
    
    message = (pwm_id << 28) |  (value & 0xFFFF)
    ino.write(struct.pack('<I', message))

send(0, 485) #right
time.sleep(0.1)
send(1, 458) #left
time.sleep(0.1)
'''
# for zero finding
v = 350
while True:
    print("value: ", v)
    str  = input("")
    send(0, v)
    time.sleep(0.1)
    send(1, v)
    time.sleep(0.1)
    v += 5
'''
