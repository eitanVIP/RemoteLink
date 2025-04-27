#include <iostream>
#include <mutex>
#include <queue>
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
    TCPHeader tcpHeader;
    IPAddress clientAddress;
    int clientPort;
    
    bool connected = false;
    bool requested = false;

    mutex mtx;
    queue<string> dataQueue;

    int Start(int port)
    {
        Utils::CreateSocket(&sock, TRUE);
        
        // sockaddr_in addr = Utils::StringToAddress("0.0.0.0", port);
        // addr.sin_addr.s_addr = INADDR_ANY;
        sockaddr_in addr = IPAddress("0.0.0.0").GetAsNetworkStruct();
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        
        //Binding socket to pc's ip address and port
        int binderr = bind(sock, (sockaddr*)&addr, sizeof(addr));
        if (binderr == SOCKET_ERROR)
        {
            Application::Log("Socket did not bind to PC on port " + to_string(port) + " because: " + Utils::GetWSAErrorString(), TRUE);
            return 1;
        }
        Application::Log("Socket bound to PC successfully on port " + to_string(port), TRUE);
        
        if (TCPNetwork::ServerHandshakeStep1(sock, tcpHeader, port, &clientAddress, &clientPort) != 0)
            return 1;
        
        requested = true;
        
        Application::Log("Received Connection request", TRUE);
        return 0;
    }

    bool IsRequested(IPAddress* client)
    {
        if (requested)
            *client = clientAddress;

        return requested;
    }

    int AcceptConnection()
    {
        if (!requested)
            return 1;

        if (TCPNetwork::ServerHandshakeStep2(sock, tcpHeader, clientAddress, clientPort) != 0)
            return 1;

        connected = true;
        requested = false;
        
        Application::Log("Connection established", TRUE);
        return 0;
    }

    bool IsConnected()
    {
        return connected;
    }

    int Update() {
        return 0;
    }

    void Close()
    {
        requested = false;
        connected = false;
        closesocket(sock);
        WSACleanup();
    }
}
