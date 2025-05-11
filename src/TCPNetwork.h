#pragma once

#include <cstdint>
#include "IPHeader.h"
#include "TCPHeader.h"
#include <iostream>
#include "IPAddress.h"

using namespace std;

namespace TCPNetwork
{
    int SendData(int sourceSocket, string data, TCPHeader& sourceTCPHeader, IPAddress destIP, bool isServer);
    int ReceiveData(int receivingSocket, TCPHeader& receivingTCPHeader, string* receivedData, IPAddress* senderIP, bool isServer);
    int ClientHandshake(int sock, TCPHeader& tcpHeader, IPAddress destIP, NetworkNumber<unsigned short>  clientPort);
    int ServerHandshakeStep1(int sock, TCPHeader& tcpHeader, NetworkNumber<unsigned short>  serverPort, IPAddress* clientAddress);
    int ServerHandshakeStep2(int sock, TCPHeader& tcpHeader, IPAddress clientAddress);
}
