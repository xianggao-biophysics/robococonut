/*
compile command on Windows:
g++ -o server server.cpp -lws2_32
*/
#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // Link with ws2_32.lib

#define PORT 5000

void handle_client(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) break;  // Connection closed

        buffer[bytesReceived] = '\0';  // Null-terminate the string
        std::cout << "Received: " << buffer << std::endl;
    }
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Binding failed" << std::endl;
        return 1;
    }

    listen(serverSocket, 5);
    std::cout << "Server listening on port " << PORT << std::endl;

    std::vector<std::thread> threads;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) continue;

        std::cout << "Client connected!" << std::endl;
        threads.emplace_back(handle_client, clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}