#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#include "Application.h"
#include "TCPHeader.h"
#include "TCPNetwork.h"
#include "Utils.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

namespace Server
{
    SOCKET sock = INVALID_SOCKET;
    bool connected = false;
    TCPHeader tcpHeader;

    int Start(int port)
    {
        Utils::CreateSocket(sock, true);

        // sockaddr_in addr = Utils::StringToAddress("0.0.0.0", port);
        // addr.sin_addr.s_addr = INADDR_ANY;
        sockaddr_in addr = IPAddress("0.0.0.0").GetAsNetworkStruct();
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        //Binding socket to pc's ip address and port
        int binderr = bind(sock, (sockaddr*)&addr, sizeof(addr));
        if (binderr == SOCKET_ERROR)
        {
            Application::Log("Socket did not bind to PC: " + Utils::GetWSAErrorString(), TRUE);
            return 1;
        }
        Application::Log("Socket bound to PC successfully", TRUE);
        
        if (TCPNetwork::ServerConnectionDance(sock, tcpHeader, port) != 0)
            return 1;

        connected = true;
        Application::Log("Connected successfuly", TRUE);
        return 0;
    }

    int Update() {
        return 0;
    }

    bool IsConnected()
    {
        return connected;
    }

    void Close()
    {
        connected = false;
        closesocket(sock);
        WSACleanup();
    }
}
