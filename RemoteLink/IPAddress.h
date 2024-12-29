#pragma once

#include <iostream>
#include <WinSock2.h>

using namespace std;

class IPAddress
{
private:
    string IP;
    sockaddr_in ipStruct;
    uint32_t ipBinary;

public:
    IPAddress(string IP);
    IPAddress(sockaddr_in IP);
    IPAddress(uint32_t IP);
    string GetAsString();
    sockaddr_in GetAsNetworkStruct();
    uint32_t GetAsNetworkBinary();
};
