#include "Client.h"

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Application.h"
#include "Utils.h"

using namespace std;

namespace Client
{
    SOCKET sock = INVALID_SOCKET;
    bool connected = false;
    
    int Setup()
    {
        //Startup WSA
        WSADATA wsaData;
        int WSAerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (WSAerr != 0)
        {
            Application::Log("[Client] WSAStartup failed with error: " + Utils::GetWSAErrorString());
            return 1;
        }

        Application::Log("[Client] WSAStartup successful");
        return 0;
    }
    
    int Connect(string address, int port)
    {
        //Create the socket
        sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
        if (sock == INVALID_SOCKET)
        {
            Application::Log("[Client] Invalid socket: " + Utils::GetWSAErrorString());
            return 1;
        }

        //Create wsa address struct
        sockaddr_in addr;
        //Using IPv4
        addr.sin_family = AF_INET;
        //Convert string address to network presentation
        inet_pton(AF_INET, address.c_str(), &addr.sin_addr.s_addr);
        //Convert port to network presentation
        addr.sin_port = htons(port);

        //Connect to address
        int err = connect(sock, (sockaddr*)&addr, sizeof(addr));
        if (err != 0)
        {
            Application::Log("[Client] Connection error: " + Utils::GetWSAErrorString());
            return 1;
        }

        connected = true;
        Application::Log("[Client] Connected successfuly");
        return 0;
    }

    void SendData(const string& data)
    {
        send(sock, data.c_str(), data.size(), 0);
    }

    void Update()
    {
        SendData("Sigma");
    }

    bool IsConnected()
    {
        return connected;
    }

    void Disconnect()
    {
        connected = false;
        closesocket(sock);
        WSACleanup();
    }
}
