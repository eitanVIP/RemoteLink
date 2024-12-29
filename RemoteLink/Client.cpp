#include "Client.h"

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Application.h"
#include "IPHeader.h"
#include "TCPHeader.h"
#include "TCPNetwork.h"
#include "Utils.h"

using namespace std;

namespace Client
{
    SOCKET sock = INVALID_SOCKET;
    bool connected = false;
    TCPHeader tcpHeader;
    
    int Connect(IPAddress address, int port)
    {
        Utils::CreateSocket(sock, false);
        // sockaddr_in addr = Utils::StringToAddress(address, port);
        sockaddr_in addr = address.GetAsNetworkStruct();
        addr.sin_port = htons(port);
        
        //Connect to address
        int err = connect(sock, (sockaddr*)&addr, sizeof(addr));
        if (err != 0)
        {
            Application::Log("[Client] Connection error: " + Utils::GetWSAErrorString());
            return 1;
        }

        if (TCPNetwork::ClientConnectionDance(sock, tcpHeader, address, port) != 0)
            return 1;

        connected = true;
        Application::Log("Connected successfuly", FALSE);
        return 0;
    }

    int Update()
    {
        return 0;
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
