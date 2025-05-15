#pragma once

#include <string>
#include "IPAddress.h"
#include "NetworkNumber.h"
#include "TCPPacket.h"

class Socket {
private:
    int sock;

public:
    int CreateSocket();
    int Bind(NetworkNumber<Port> port);
    int Connect(IPAddress IP);
    int SendPacket(TCPPacket tcpPacket, IPAddress destIP) const;
    int ReceivePacket(TCPPacket* receivedData, IPAddress* senderIP, NetworkNumber<Port> port) const;
    void Close();
};
