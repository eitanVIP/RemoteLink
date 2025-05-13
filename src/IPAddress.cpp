#include "IPAddress.h"

#include <cstring>
#include <sstream>
#include <arpa/inet.h>
#include "Socket.h"

IPAddress::IPAddress()
{
    IP = "0.0.0.0";
    port = NetworkNumber<Port>(0, NumberType::Host);

    inet_pton(AF_INET, IP.c_str(), &ipStruct.sin_addr);
    ipStruct.sin_port = port.GetAsNetwork(); // Set port to 0 as we don't need it
    ipStruct.sin_family = AF_INET;

    ipBinary = *(uint32_t*)&ipStruct.sin_addr;
}

IPAddress::IPAddress(string ip, NetworkNumber<Port> port)
{
    IP = ip;
    this->port = port;

    inet_pton(AF_INET, IP.c_str(), &ipStruct.sin_addr);
    ipStruct.sin_port = port.GetAsNetwork();
    ipStruct.sin_family = AF_INET;

    ipBinary = *(uint32_t*)&ipStruct.sin_addr;
}

IPAddress::IPAddress(sockaddr_in ip)
{
    ipStruct = ip;
    ipBinary = *(uint32_t*)&ipStruct.sin_addr;
    this->port = NetworkNumber(ip.sin_port, NumberType::Network);

    char buffer[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &ipStruct.sin_addr, buffer, INET_ADDRSTRLEN);
    IP = buffer; // Assign to std::string
}

IPAddress::IPAddress(uint32_t ip, NetworkNumber<Port> port)
{
    ipBinary = ip;
    ipStruct.sin_addr.s_addr = ip;
    ipStruct.sin_port = port.GetAsNetwork();
    ipStruct.sin_family = AF_INET;
    this->port = port;

    char buffer[INET_ADDRSTRLEN] = {0}; // Buffer for IPv4 address string
    inet_ntop(AF_INET, &ipStruct.sin_addr, buffer, INET_ADDRSTRLEN);
    IP = buffer; // Assign to std::string
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

NetworkNumber<unsigned short> IPAddress::GetPort()
{
    return port;
}