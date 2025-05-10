#include "Client.h"

#include <iostream>
#include <string>
#include "Application.h"
#include "IPHeader.h"
#include "TCPHeader.h"
#include "TCPNetwork.h"
#include "Utils.h"

using namespace std;

namespace Client
{
    int sock = -1;
    bool connected = false;
    TCPHeader tcpHeader;
    IPAddress serverAddress;
    int serverPort;
    
    int Connect(IPAddress address, int port)
    {
        Utils::CreateSocket(&sock, false);

        serverAddress = address;
        serverPort = port;
        sockaddr_in addr = address.GetAsNetworkStruct();
        addr.sin_port = htons(port);
        
        //Connect to address
        int err = connect(sock, (sockaddr*)&addr, sizeof(addr));
        if (err != 0)
        {
            Application::Log("Connection error", false);
            return 1;
        }
        Application::Log("Found server with IP: " + address.GetAsString(), false);

        if (TCPNetwork::ClientHandshake(sock, tcpHeader, address, port, port/*Temporary*/) != 0)
            return 1;

        connected = true;
        Application::Log("Connection established", false);
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
        return TCPNetwork::SendData(sock, message, tcpHeader, serverAddress, serverPort, false);
    }

    void Disconnect()
    {
        connected = false;
        close(sock);
    }
}
