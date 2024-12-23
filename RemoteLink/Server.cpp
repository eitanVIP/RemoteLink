#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

namespace Server
{
    WSADATA wsaData;
    SOCKET serverSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;
    sockaddr_in serverAddr = {};
    sockaddr_in clientAddr = {};
    bool isInitialized = false;

    int Start(int port)
    {
        // Init Winsock
        WORD wVersionRequested = MAKEWORD(2, 2);
        int wsaerr = WSAStartup(wVersionRequested, &wsaData);

        if (wsaerr != 0)
        {
            cerr << "The Winsock DLL was not found or failed to load. Error: " << wsaerr << endl;
            return -1;
        }

        cout << "The Winsock DLL loaded successfully.\n";
        cout << "Status: " << wsaData.szSystemStatus << endl;

        // Create a socket
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET)
        {
            cerr << "Socket creation failed. Error: " << WSAGetLastError() << endl;
            WSACleanup();
            return -1;
        }

        // Bind the socket
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
        {
            cerr << "Socket binding failed. Error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return -1;
        }

        // Start listening
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            cerr << "Socket listen failed. Error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return -1;
        }

        cout << "Server is listening on port " << port << endl;

        isInitialized = true;
        return 0;
    }

    void Update() {
        if (!isInitialized)
            return;

        // Accept a client if not already connected
        if (clientSocket == INVALID_SOCKET) {
            int clientAddrLen = sizeof(clientAddr);
            clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);

            if (clientSocket != INVALID_SOCKET) {
                // Successfully accepted a client, now we can use clientAddr
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
                cout << "Client connected from IP: " << clientIP << endl;
            }
        }

        // If a client is connected, check for data
        if (clientSocket != INVALID_SOCKET) {
            char buffer[1024] = {};  // Buffer to store incoming data
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';  // Null-terminate the received data
                cout << "Received data: " << buffer << endl;

                // Process the received data
                int number = atoi(buffer);
                if (number == 42) {
                    cout << "Received number: 42" << endl;
                }
            } else if (bytesReceived == 0) {
                // Connection closed by client
                cout << "Client disconnected." << endl;
                closesocket(clientSocket);
                clientSocket = INVALID_SOCKET;
            } else {
                // Error in receiving data
                cerr << "Error receiving data: " << WSAGetLastError() << endl;
            }
        }
    }

    bool IsConnected()
    {
        return clientSocket != INVALID_SOCKET;
    }

    bool IsInitialized()
    {
        return isInitialized;
    }

    void Close()
    {
        if (clientSocket != INVALID_SOCKET)
            closesocket(clientSocket);

        if (serverSocket != INVALID_SOCKET)
            closesocket(serverSocket);

        WSACleanup();
        isInitialized = false;
    }
}
