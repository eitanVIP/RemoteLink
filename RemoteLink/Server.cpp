#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#include "Application.h"
#include "Utils.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

namespace Server
{
    SOCKET listenSock = INVALID_SOCKET;
    bool isConnected = false;
    
    int Start(int port)
    {
        //Startup WSA
        WSADATA wsaData;
        int WSAerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (WSAerr != 0)
        {
            Application::Log("[Server] WSAStartup failed with error: " + Utils::GetWSAErrorString());
            return 1;
        }
        Application::Log("[Server] WSAStartup successful");

        //Create the socket
        listenSock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
        if (listenSock == INVALID_SOCKET)
        {
            Application::Log("[Server] Invalid socket: " + Utils::GetWSAErrorString());
            return 1;
        }

        //Set socket to non-blocking mode- doesn't wait for data, if there isn't data it just continues the code
        u_long mode = 1; // non-blocking mode
        ioctlsocket(listenSock, FIONBIO, &mode);

        //Create wsa address struct
        sockaddr_in addr;
        //Using IPv4
        addr.sin_family = AF_INET;
        //Convert string address to network presentation
        addr.sin_addr.s_addr = INADDR_ANY;
        //Convert port to network presentation
        addr.sin_port = htons(port);

        //Binding socket to pc's ip address and port
        int binderr = bind(listenSock, (sockaddr*)&addr, sizeof(addr));
        if (binderr == SOCKET_ERROR)
        {
            Application::Log("[Server] Socket did not bind to PC: " + Utils::GetWSAErrorString());
            return 1;
        }

        Application::Log("[Server] Binded socket to PC successfully");
        return 0;
    }

    void Update() {
        char buffer[1024];
        int data = recv(listenSock, buffer, sizeof(buffer), 0);
        
        if (data > 0) {
            Application::Log("[Server] Packet received");
        } else if (data == SOCKET_ERROR)
            if (WSAGetLastError() != WSAEWOULDBLOCK)
                Application::Log("[Server] recv failed: " + Utils::GetWSAErrorString());
    }

    bool IsConnected()
    {
        return isConnected;
    }

    void Close()
    {
        isConnected = false;
        closesocket(listenSock);
        WSACleanup();
    }
}
