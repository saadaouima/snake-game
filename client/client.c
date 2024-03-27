#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    char recvbuf[BUFFER_SIZE];
    int iResult;
    int recvbuflen = BUFFER_SIZE;

    // Initialize Winsock
    printf("Initializing Winsock...\n");
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        error("WSAStartup failed");
    }
    printf("Winsock initialized.\n");

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    printf("Resolving server address...\n");
    iResult = getaddrinfo("127.0.0.1", "27015", &hints, &result);
    if (iResult != 0) {
        error("getaddrinfo failed");
    }
    printf("Server address resolved.\n");

    // Attempt to connect to an address until one succeeds
    printf("Attempting to connect to server...\n");
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            error("socket failed");
        }

        // Connect to server
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        error("Unable to connect to server");
    }
    printf("Connected to server.\n");

    // Receive until the peer closes the connection
    printf("Receiving data from server...\n");
    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Received from server: %.*s\n", iResult, recvbuf);
        } else if (iResult == 0) {
            printf("Connection closed by server\n");
        } else {
            error("recv failed");
        }
    } while (iResult > 0);

    // Cleanup
    printf("Closing socket and cleaning up...\n");
    closesocket(ConnectSocket);
    WSACleanup();

    printf("Press Enter to exit...");
    getchar(); // Wait for user to press Enter
    return 0;
}
