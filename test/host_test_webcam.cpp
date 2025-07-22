#include <opencv2/opencv.hpp>
#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // Link with ws2_32.lib

#define PORT 5000

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Binding failed!" << std::endl;
        return 1;
    }

    listen(serverSocket, 1);
    std::cout << "Server listening on port " << PORT << std::endl;

    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to accept client connection!" << std::endl;
        return 1;
    }

    std::cout << "Client connected, receiving image..." << std::endl;

    int imgSize;
    recv(clientSocket, (char*)&imgSize, sizeof(imgSize), 0);  // Receive image size

    std::vector<uchar> buffer(imgSize);
    recv(clientSocket, (char*)buffer.data(), imgSize, 0);  // Receive image data

    cv::Mat img = cv::imdecode(buffer, cv::IMREAD_COLOR);  // Convert back to image
    if (img.empty()) {
        std::cerr << "Error: Received empty image!" << std::endl;
        return 1;
    }

    cv::imshow("Received Image", img);
    cv::waitKey(0);

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
