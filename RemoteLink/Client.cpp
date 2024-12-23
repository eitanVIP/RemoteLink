#include "Client.h"

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

namespace Client
{
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;

    int Connect(const char* hostAddr, int port)
    {
        // Initialize Winsock
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
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET)
        {
            cerr << "Socket creation failed. Error: " << WSAGetLastError() << endl;
            WSACleanup();
            return -1;
        }

        // Resolve the server address and port
        sockaddr_in serverAddr = {};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);

        if (inet_pton(AF_INET, hostAddr, &serverAddr.sin_addr) <= 0)
        {
            cerr << "Invalid address or address not supported: " << hostAddr << endl;
            closesocket(sock);
            WSACleanup();
            return -1;
        }

        // Connect to the server
        if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
        {
            cerr << "Connection failed. Error: " << WSAGetLastError() << endl;
            closesocket(sock);
            WSACleanup();
            return -1;
        }

        cout << "Connected to the server at " << hostAddr << ":" << port << endl;

        return 0;
    }

    int SendData(const string& data)
    {
        if (sock == INVALID_SOCKET)
        {
            cerr << "Socket is not connected!" << endl;
            return -1;
        }

        int result = send(sock, data.c_str(), static_cast<int>(data.length()), 0);
        if (result == SOCKET_ERROR)
        {
            cerr << "Failed to send data. Error: " << WSAGetLastError() << endl;
            return -1;
        }

        cout << "Data sent: " << data << endl;
        return result;
    }

    int Update()
    {
        POINT mousePos;
        if (GetCursorPos(&mousePos))
            return SendData("x: " + to_string(mousePos.x) + ", y: " + to_string(mousePos.y));
        
        return 0;
    }

    bool IsConnected()
    {
        return sock != INVALID_SOCKET;
    }

    void Disconnect()
    {
        if (sock != INVALID_SOCKET)
        {
            closesocket(sock);
            sock = INVALID_SOCKET;
        }
        WSACleanup();
    }
}
