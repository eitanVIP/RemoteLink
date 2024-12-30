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
    int SendData(SOCKET sock, string data, TCPHeader& tcpHeader, IPAddress destIP, BOOL isServer);
    int RecieveData(SOCKET sock, TCPHeader& tcpHeader, BOOL isServer, string* recievedData, IPAddress* senderIP, int* senderPort);
    int ClientHandshake(SOCKET sock, TCPHeader& tcpHeader, IPAddress destIP, int port);
    int ServerHandshakeStep1(SOCKET sock, TCPHeader& tcpHeader, int port, IPAddress* clientAddress);
    int ServerHandshakeStep2(SOCKET sock, TCPHeader& tcpHeader, IPAddress clientAddress);
}
