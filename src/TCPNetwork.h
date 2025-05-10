#pragma once

#include <cstdint>
#include "IPHeader.h"
#include "TCPHeader.h"
#include <iostream>
#include "IPAddress.h"

using namespace std;

namespace TCPNetwork
{
    int SendData(int sourceSocket, string data, TCPHeader& sourceTCPHeader, IPAddress destIP, int destPort, bool isServer);
    int ReceiveData(int receivingSocket, TCPHeader& receivingTCPHeader, string* receivedData, IPAddress* senderIP, int* senderPort, bool isServer);
    int ClientHandshake(int sock, TCPHeader& tcpHeader, IPAddress destIP, int destPort, int clientPort);
    int ServerHandshakeStep1(int sock, TCPHeader& tcpHeader, int serverPort, IPAddress* clientAddress, int* clientPort);
    int ServerHandshakeStep2(int sock, TCPHeader& tcpHeader, IPAddress clientAddress, int clientPort);
}
