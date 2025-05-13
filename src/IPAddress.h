#pragma once

#include <iostream>
#include <netinet/in.h>
#include "NetworkNumber.h"
#include "PortTypeDef.h"

using namespace std;

class IPAddress
{
private:
    string IP;
    sockaddr_in ipStruct;
    uint32_t ipBinary;
    NetworkNumber<Port> port = NetworkNumber<Port>(0, NumberType::Host);

public:
    IPAddress();
    IPAddress(string IP, NetworkNumber<Port> port);
    IPAddress(sockaddr_in IP);
    IPAddress(uint32_t IP, NetworkNumber<Port> port);
    string GetAsString();
    sockaddr_in GetAsNetworkStruct();
    uint32_t GetAsNetworkBinary();
    NetworkNumber<Port> GetPort();
};
