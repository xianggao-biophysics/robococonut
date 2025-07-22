import serial
import struct
import time
from time import sleep

# Define Arduino serial port (adjust as needed)
# SERIAL_PORT = '/dev/ttyUSB0'  # Change for Windows (e.g., 'COM3')
SERIAL_PORT = 'COM3'  # Change for Windows (e.g., 'COM3')
BAUD_RATE = 115200

# Message type definitions
INIT_TYPE = 0xF
SERVO_TYPE = 0xE
MOTOR_PWM_TYPE = 0xD
MOTOR_DIR_TYPE = 0xC

# Device indices
DEV_1 = 1
DEV_2 = 2

# Data constraints
SERVO_MIN = 0
SERVO_MAX = 1000
MOTOR_MIN = 0
MOTOR_MAX = 255

def send_command(ser, msg_type, device, data):
    """Send a 32-bit command to Arduino."""
    command = (msg_type << 24) | (device << 16) | (data & 0xFFFF)
    packed_data = struct.pack('<I', command)  # Pack as little-endian uint32_t
    ser.write(packed_data)
    response = ser.read(1)  # Read 1 byte response
    return response

def initialize(ser):
    """Test connection with Arduino."""
    response = send_command(ser, INIT_TYPE, 0, 0)
    return response == b'\x01'

def move_servo(ser, device, position):
    """Move servo to position (0-1000)."""
    position = max(SERVO_MIN, min(SERVO_MAX, position))
    response = send_command(ser, SERVO_TYPE, device, position)
    return response == b'\x02'

def set_motor_speed(ser, device, speed):
    """Set motor speed (0-255)."""
    speed = max(MOTOR_MIN, min(MOTOR_MAX, speed))
    response = send_command(ser, MOTOR_PWM_TYPE, device, speed)
    return response == b'\x03'

def set_motor_direction(ser, device, direction):
    """Set motor direction (0 for one way, 1 for the other)."""
    direction = 1 if direction else 0
    response = send_command(ser, MOTOR_DIR_TYPE, device, direction)
    return response == b'\x04'

if __name__ == "__main__":
    with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
        time.sleep(2)  # Allow time for Arduino to reset
        
        if initialize(ser):
            print("Arduino initialized successfully.")
        else:
            print("Initialization failed.")

        # Example commands
        testbit = True
        dev = DEV_2
        while True:
            print(move_servo(ser, dev, 1000))  # Move servo 1 to mid-position
            print(set_motor_speed(ser, dev, 255))  # Set motor 1 speed to 50%
            print(set_motor_direction(ser, dev, 1))  # Set motor 1 direction forward

            
            # print(1000 if testbit else 0)
            # print(move_servo(ser, dev, 1000 if testbit else 0))  # Move servo 1 to mid-position
            # print(set_motor_speed(ser, dev, 255 if testbit else 0))  # Set motor 1 speed to 50%
            # print(set_motor_direction(ser, dev, testbit))  # Set motor 1 direction forward
            

            testbit = not testbit
            sleep(1)
            # set_motor_speed(ser, DEV_2, 255)  # Set motor 1 speed to 50%
            # set_motor_direction(ser, DEV_2, 1)  # Set motor 1 direction forward
