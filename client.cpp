#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

const char* SERVER_URL = "hallowed-lime-raccoon.glitch.me";
const int PORT = 80; // HTTP port

int main() {
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_URL, &hint.sin_addr);

    connect(sock, (sockaddr*)&hint, sizeof(hint));

    // Send a simple HTTP GET request
    const char* request = "GET / HTTP/1.1\r\nHost: hallowed-lime-raccoon.glitch.me\r\nConnection: close\r\n\r\n";
    send(sock, request, strlen(request), 0);

    // Receive the response
    char buffer[4096];
    int bytesReceived = recv(sock, buffer, 4096, 0);
    if (bytesReceived > 0) {
        std::cout << std::string(buffer, 0, bytesReceived) << std::endl;
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}

}

