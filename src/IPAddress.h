#pragma once

#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <arpa/inet.h>  // inet_pton, inet_ntop
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // socket types
#include <unistd.h>
#include <cstring>

using namespace std;

class IPAddress
{
private:
    string IP;
    sockaddr_ll ipStruct;
    uint32_t ipBinary;

public:
    IPAddress();
    IPAddress(string IP);
    IPAddress(sockaddr_ll IP);
    IPAddress(uint32_t IP);
    string GetAsString();
    sockaddr_ll GetAsNetworkStruct();
    uint32_t GetAsNetworkBinary();
};
