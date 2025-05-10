#pragma once

#include <iostream>
#include <cstring>
#include <arpa/inet.h>  // For inet_pton
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For close()

using namespace std;

class IPAddress
{
private:
    string IP;
    sockaddr_in ipStruct;
    uint32_t ipBinary;

public:
    IPAddress();
    IPAddress(string IP);
    IPAddress(sockaddr_in IP);
    IPAddress(uint32_t IP);
    string GetAsString();
    sockaddr_in GetAsNetworkStruct();
    uint32_t GetAsNetworkBinary();
};
