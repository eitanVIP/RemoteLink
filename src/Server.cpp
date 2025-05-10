#include "Server.h"

#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "Application.h"
#include "TCPHeader.h"
#include "TCPNetwork.h"
#include "Utils.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

namespace Server
{
    int sock = -1;
    TCPHeader tcpHeader;
    IPAddress clientAddress;
    int clientPort;
    
    bool connected = false;
    bool requested = false;

    mutex mtx;
    queue<string> dataQueue;

    int Start(int port)
    {
        Utils::CreateSocket(&sock, true);

        sockaddr_in addr = IPAddress("0.0.0.0").GetAsNetworkStruct();
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        
        //Binding socket to pc's ip address and port
        int binderr = bind(sock, (sockaddr*)&addr, sizeof(addr));
        if (binderr < 0)
        {
            Application::Log("Socket did not bind to PC on port " + to_string(port), true);
            return 1;
        }
        Application::Log("Socket bound to PC successfully on port " + to_string(port), true);
        
        if (TCPNetwork::ServerHandshakeStep1(sock, tcpHeader, port, &clientAddress, &clientPort) != 0)
            return 1;
        
        requested = true;
        
        Application::Log("Received Connection request", true);
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
        
        Application::Log("Connection established", true);

        thread serverUpdate([]()
        {
            while (Update() == 0);
        });
        serverUpdate.detach();
        
        return 0;
    }

    bool IsConnected()
    {
        return connected;
    }

    int Update() {
        string data;
        IPAddress sender;
        int senderPort;
        return TCPNetwork::ReceiveData(sock, tcpHeader, &data, &sender, &senderPort, true);
    }

    void Close()
    {
        requested = false;
        connected = false;
        close(sock);
    }
}
