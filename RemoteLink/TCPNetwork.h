#pragma once

#include <cstdint>
#include "IPHeader.h"
#include "TCPHeader.h"
#include <iostream>
#include <winsock2.h>
#include "IPAddress.h"

using namespace std;

namespace TCPNetwork
{
    int SendData(SOCKET sourceSocket, string data, TCPHeader& sourceTCPHeader, IPAddress destIP, int destPort, BOOL isServer);
    int ReceiveData(SOCKET receivingSocket, TCPHeader& receivingTCPHeader, string* receivedData, IPAddress* senderIP, int* senderPort, BOOL isServer);
    int ClientHandshake(SOCKET sock, TCPHeader& tcpHeader, IPAddress destIP, int destPort, int clientPort);
    int ServerHandshakeStep1(SOCKET sock, TCPHeader& tcpHeader, int serverPort, IPAddress* clientAddress, int* clientPort);
    int ServerHandshakeStep2(SOCKET sock, TCPHeader& tcpHeader, IPAddress clientAddress, int clientPort);
}
