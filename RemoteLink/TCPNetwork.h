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
    IPHeader CreateIPHeader(uint32_t srcIP, uint32_t destIP, uint16_t dataLength);
    TCPHeader CreateInitialTCPHeader(int port);
    int SendData(SOCKET sock, string data, TCPHeader& tcpHeader, IPAddress destIP, BOOL isServer);
    int RecieveData(SOCKET sock, TCPHeader& tcpHeader, BOOL isServer, string& recievedData, IPAddress& senderIP, int& senderPort);
    int ClientConnectionDance(SOCKET sock, TCPHeader& tcpHeader, IPAddress destIP, int port);
    int ServerConnectionDance(SOCKET sock, TCPHeader& tcpHeader, int myPort);
}
