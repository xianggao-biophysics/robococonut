#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "10.0.0.229"  // Replace with your Windows PC IP
#define PORT 5000

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Connection failed" << std::endl;
        return 1;
    }

    std::cout << "Connected to server. Sending message..." << std::endl;
    std::string message = "Hello from Raspberry Pi!";
    send(sock, message.c_str(), message.size(), 0);

    close(sock);
    return 0;
}
