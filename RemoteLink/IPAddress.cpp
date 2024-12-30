#include "IPAddress.h"
#include <WinSock2.h>
#include <sstream>
#include <ws2tcpip.h>

IPAddress::IPAddress()
{
    IP = "";
    // Convert the string IP to network byte order and store it in ipStruct
    inet_pton(AF_INET, IP.c_str(), &ipStruct.sin_addr);  
    ipStruct.sin_port = 0; // Set port to 0 as we don't need it
    ipStruct.sin_family = AF_INET;
    
    ipBinary = *(uint32_t*)&ipStruct.sin_addr;
}

IPAddress::IPAddress(string ip)
{
    IP = ip;
    // Convert the string IP to network byte order and store it in ipStruct
    inet_pton(AF_INET, IP.c_str(), &ipStruct.sin_addr);  
    ipStruct.sin_port = 0; // Set port to 0 as we don't need it
    ipStruct.sin_family = AF_INET;
    
    ipBinary = *(uint32_t*)&ipStruct.sin_addr;
}

IPAddress::IPAddress(sockaddr_in ip)
{
    ipStruct = ip;
    ipStruct.sin_port = 0; // Set port to 0 as we don't need it
    ipBinary = *(uint32_t*)&ipStruct.sin_addr;
    
    char buffer[INET_ADDRSTRLEN] = {0}; // Buffer for IPv4 address string
    inet_ntop(AF_INET, &ipStruct.sin_addr, buffer, INET_ADDRSTRLEN);
    IP = buffer; // Assign to std::string
}

IPAddress::IPAddress(uint32_t ip)
{
    ipBinary = ip;
    ipStruct.sin_addr.s_addr = ip;  
    ipStruct.sin_port = 0; // Set port to 0 as we don't need it
    ipStruct.sin_family = AF_INET;
    
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
