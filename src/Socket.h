#pragma once

#include <string>
#include "IPAddress.h"
#include "NetworkNumber.h"
#include "TCPHeader.h"

class Socket {
private:
    int sock;
    TCPHeader tcpHeader;

public:
    int CreateSocket();
    int Bind(NetworkNumber<Port> port);
    int Connect(IPAddress IP);
    int SendData(std::string data, IPAddress destIP);
    int ReceiveData(string* receivedData, IPAddress* senderIP, bool receiveFromAll);
    TCPHeader& GetTCPHeader();
    void Close();
};