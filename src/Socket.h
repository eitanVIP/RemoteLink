#pragma once
#include <string>
#include "IPAddress.h"
#include "NetworkNumber.h"
#include "TCPHeader.h"

typedef unsigned short Port;

class Socket {
private:
    int socket;
    TCPHeader tcpHeader;

public:
    Socket();
    bool IsSocketValid();
    int Bind(NetworkNumber<Port> port);
    int Connect(IPAddress IP);
    int SendData(std::string data, IPAddress destIP);
    int ReceiveData(string* receivedData, IPAddress* senderIP, bool receiveFromAll);
    TCPHeader& GetTCPHeader();
    void Close();
};