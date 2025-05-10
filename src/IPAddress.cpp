#include "IPAddress.h"
#include <sstream>

IPAddress::IPAddress()
{
    IP = "0.0.0.0";
    memset(&ipStruct, 0, sizeof(ipStruct));
    ipStruct.sin_family = AF_INET;
    inet_pton(AF_INET, IP.c_str(), &ipStruct.sin_addr);
    ipStruct.sin_port = 0;

    ipBinary = ipStruct.sin_addr.s_addr;
}

IPAddress::IPAddress(string ip)
{
    IP = ip;
    memset(&ipStruct, 0, sizeof(ipStruct));
    ipStruct.sin_family = AF_INET;
    inet_pton(AF_INET, IP.c_str(), &ipStruct.sin_addr);
    ipStruct.sin_port = 0;

    ipBinary = ipStruct.sin_addr.s_addr;
}

IPAddress::IPAddress(sockaddr_in ip)
{
    ipStruct = ip;
    ipStruct.sin_port = 0;
    ipBinary = ipStruct.sin_addr.s_addr;

    char buffer[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &ipStruct.sin_addr, buffer, INET_ADDRSTRLEN);
    IP = buffer;
}

IPAddress::IPAddress(uint32_t ip)
{
    ipBinary = ip;
    memset(&ipStruct, 0, sizeof(ipStruct));
    ipStruct.sin_addr.s_addr = ip;
    ipStruct.sin_family = AF_INET;
    ipStruct.sin_port = 0;

    char buffer[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &ipStruct.sin_addr, buffer, INET_ADDRSTRLEN);
    IP = buffer;
}

string IPAddress::GetAsString()
{
    return IP;
}

sockaddr_in IPAddress::GetAsNetworkStruct()
{
    return ipStruct;
}

uint32_t IPAddress::GetAsNetworkBinary()
{
    return ipBinary;
}
