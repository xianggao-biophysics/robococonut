#include <opencv2/opencv.hpp>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "10.0.0.229"  // Replace with your Windows PC's IP
#define PORT 5000

int main() {
    cv::VideoCapture cap(0);  // Open the default camera
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open the camera!" << std::endl;
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Connection failed!" << std::endl;
        return 1;
    }

    std::cout << "Connected to server, capturing image..." << std::endl;
    
    cv::Mat frame;
    cap >> frame;  // Capture an image
    if (frame.empty()) {
        std::cerr << "Error: Captured empty frame!" << std::endl;
        return 1;
    }

    std::vector<uchar> buffer;
    cv::imencode(".jpg", frame, buffer);  // Convert image to byte array

    int imgSize = buffer.size();
    send(sock, &imgSize, sizeof(imgSize), 0);  // Send image size first
    send(sock, buffer.data(), imgSize, 0);  // Send image data

    std::cout << "Image sent to server!" << std::endl;
    
    close(sock);
    return 0;
}
