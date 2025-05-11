#pragma once

#include <iostream>
#include <netinet/in.h>
#include "NetworkNumber.h"

using namespace std;

class IPAddress
{
private:
    string IP;
    sockaddr_in ipStruct;
    uint32_t ipBinary;
    NetworkNumber<unsigned short> port = NetworkNumber<unsigned short>(0, NumberType::Host);

public:
    IPAddress();
    IPAddress(string IP, NetworkNumber<unsigned short> port);
    IPAddress(sockaddr_in IP);
    IPAddress(uint32_t IP, NetworkNumber<unsigned short> port);
    string GetAsString();
    sockaddr_in GetAsNetworkStruct();
    uint32_t GetAsNetworkBinary();
    NetworkNumber<unsigned short> GetPort();
};
