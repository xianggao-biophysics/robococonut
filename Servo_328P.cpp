#include <string_view>
#include "Servo_328P.hpp"

int Servo_328P::n_dev = 0; // Initialize static member variable for the number of instances created
int Servo_328P::serial_fd = -1; // Initialize static member variable for serial file descriptor
std::mutex Servo_328P::mtx {};

Servo_328P::Servo_328P(int dev_in, double min_rescale, double max_rescale) : Servo_base(dev_in, min_rescale, max_rescale) { 
    std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety when initializing the serial port
    if(dev != 0 && dev != 1) {
        std::cerr << "Error: dev doesn't exit, use dev = 0." << std::endl;
	return;
    }
    ++n_dev; // Increment the static counter for the number of instances created
    if (n_dev > 1) {
        std::cout << "Another instance of Servo_328P already exists. Not opening the serial port again.\n";
        std::cout << "New Servo_328P instance " << dev << " created, count: " << n_dev << "\n";
        return; // Only the first instance need to open the serial port. Subsequent instances will return early to avoid multiple openings of the same serial port.
    }
    if(serial_fd != -1) {
        std::cerr << "Warning: Attempting to initialize Servo_328P while serial_fd is already open. This may lead to unexpected behavior.\n";
        return; // serial fd already opened
    }
    serial_fd = open_serial();
    if (serial_fd == -1) {
        std::cerr << "Error: Failed to open serial port for Servo_328P\n";
        return; // If the serial port cannot be opened, exit the constructor
    }
    // special code for connection testing 
    unsigned int test = 0xF << 28;
    ssize_t bytes_written = write(serial_fd, &test, sizeof(test));
    if (bytes_written == -1) {
        std::cerr << "Error: Failed to write to serial port during initialization\n";
        return; // If the write operation fails, exit the constructor
    }
    usleep(100000); // Sleep for 10ms to allow the microcontroller to process the message
    char response = 0;
    int try_count = 0;
    while (read(serial_fd, &response, sizeof(response)) != sizeof(response) && try_count < 3) {
        ++try_count;
        std::cerr << "Error: Failed to read from serial port during initialization" << std::endl;
        usleep(10000); // Sleep for 10ms before retrying
    }
    if (try_count == 3) {
        std::cerr << "Error: Failed to read response after 3 attempts\n"; // do not return
    }
    if (response != 1) {
        std::cerr << "Error: Failed to read from serial port during initialization" << std::endl;
        std::cerr << "Received message:" << response << std::endl;
        return; // If the read operation fails, exit the constructor
    } 
    std::cout << "New Servo_328P instance " << dev << " created, count: " << n_dev << "\n";
}

Servo_328P::~Servo_328P() {
    std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety when closing the serial port
    --n_dev;
    if (n_dev != 0) {
        return; // Only close the serial port if this is the last instance being destroyed. This prevents closing the serial port while other instances are still using it.
    }
    if (serial_fd == -1) { 
        std::cerr << "Error: No more Servo_328P instances, but serial_fd is already closed.\n";
    } else if (close(serial_fd) == -1) {
        std::cerr << "Error: Failed to close serial port for Servo_328P\n";
    } else {
        std::cout << "Serial port closed for Servo_328P\n";
    }
}

double Servo_328P::Rotate_to(double fraction) {
    std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety when accessing the serial port
    if (fraction < 0) fraction = 0;
    if (fraction > 1) fraction = 1;
    fraction = rescale(fraction);
    uint16_t value = static_cast<uint16_t>(fraction * 1000); 
    unsigned int message = (dev << 28) | (value & 0xFFFF);
    // Close the serial port
    int try_count = 0;
    while (write(serial_fd, &message, sizeof(message)) != sizeof(message) && try_count < 3) {
        ++try_count;
        std::cerr << "Error: Failed to  write to serial port, attempt " << try_count << " of 3\n";
        usleep(10000); // Sleep for 10ms before retrying
    }
    if (try_count == 3) {
        std::cerr << "Error: Failed to send message after 3 attempts\n";
        close(serial_fd); 
        serial_fd = open_serial();
        if (serial_fd == -1) {
            std::cerr << "Error: Failed to reopen serial port\n";
            return -1;
        } else {
            std::cout << "Serial port reopened successfully\n";
        }
    }
    char response = 0;
    try_count = 0;
    while (read(serial_fd, &response, sizeof(response)) != sizeof(response) && try_count < 3) {
        ++try_count;
        std::cerr << "Error: Failed to read from serial port, attempt " << try_count << " of 3\n";
        usleep(10000); // Sleep for 10ms before retrying
    }
    if (try_count == 3) {
        std::cerr << "Error: Failed to read response after 3 attempts\n"; // do not return
    }
    if (response != 2) {
        std::cerr << "Error: Failed to read from serial port during rotation" << std::endl;
        std::cerr << "Received message:" << response << std::endl;
        return -1;
    } else {
        // std::cout << "Message sent successfully: " << std::hex << message << "\n";
    }
    return fraction;
}

int Servo_328P::open_serial() {
    constexpr std::string_view port = "/dev/ttyUSB0";  // Serial port
    int baudrate = B115200;             // Baud rate

    // Open serial port
    int serial_fd = open(port.data(), O_RDWR | O_NOCTTY);
    if (serial_fd == -1) {
        std::cerr << "Error: Failed to open serial port!\n";
        return 1;
    }

    // Configure the serial port
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(serial_fd, &tty) != 0) {
        std::cerr << "Error getting attributes!\n";
        return 1;
    }

    // Set baud rate
    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);

    // 8N1 mode (8 data bits, no parity, 1 stop bit)
    tty.c_cflag = CS8 | CLOCAL | CREAD;

    // Disable hardware flow control
    tty.c_cflag &= ~CRTSCTS;

    // Disable software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Raw mode (no processing of input/output)
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_oflag &= ~OPOST;

    // Set timeout (timeout = 1s)
    tty.c_cc[VMIN] = 0;  // Minimum number of characters to read
    tty.c_cc[VTIME] = 10; // Timeout in 1/10 seconds (1s)

    // Apply settings
    if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error setting attributes!\n";
        return -1;
    }

    std::cout << "Serial port " << port.data() << " opened at " << 115200 << " baud\n";
    return serial_fd; // Return the file descriptor for the opened serial port
}
