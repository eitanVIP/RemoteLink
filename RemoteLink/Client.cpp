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
    IPAddress serverAddress;
    int serverPort;
    
    int Connect(IPAddress address, int port)
    {
        Utils::CreateSocket(&sock, FALSE);

        serverAddress = address;
        serverPort = port;
        sockaddr_in addr = address.GetAsNetworkStruct();
        addr.sin_port = htons(port);
        
        //Connect to address
        int err = connect(sock, (sockaddr*)&addr, sizeof(addr));
        if (err != 0)
        {
            Application::Log("Connection error: " + Utils::GetWSAErrorString(), FALSE);
            return 1;
        }
        Application::Log("Found server with IP: " + address.GetAsString(), FALSE);

        if (TCPNetwork::ClientHandshake(sock, tcpHeader, address, port, port/*Temporary*/) != 0)
            return 1;

        connected = true;
        Application::Log("Connection established", FALSE);
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

    int SendMessageToServer(string message)
    {
        return TCPNetwork::SendData(sock, message, tcpHeader, serverAddress, serverPort, FALSE);
    }

    void Disconnect()
    {
        connected = false;
        closesocket(sock);
        WSACleanup();
    }
}
