#include "IPAddress.h"
#include <WinSock2.h>
#include <sstream>
#include <ws2tcpip.h>

IPAddress::IPAddress(string ip)
{
    IP = ip;
    // Convert the string IP to network byte order and store it in ipStruct
    inet_pton(AF_INET, IP.c_str(), &ipStruct.sin_addr);  
    ipStruct.sin_port = 0; // Set port to 0 as we don't need it
    ipStruct.sin_family = AF_INET;
    
    // Convert to host byte order and store in ipBinary
    ipBinary = ntohl(*(uint32_t*)&ipStruct.sin_addr);  
}

IPAddress::IPAddress(sockaddr_in ip)
{
    ipStruct = ip;
    ipStruct.sin_port = 0; // Set port to 0 as we don't need it
    // Convert to host byte order
    ipBinary = ntohl(*(uint32_t*)&ipStruct.sin_addr);
    // Convert IN_ADDR to string
    IP = inet_ntoa(ipStruct.sin_addr);  
}

IPAddress::IPAddress(uint32_t ip)
{
    ipBinary = ip;
    ipStruct.sin_addr.s_addr = htonl(ip);  
    ipStruct.sin_port = 0; // Set port to 0 as we don't need it
    ipStruct.sin_family = AF_INET;
    
    // Convert IN_ADDR to string
    IP = inet_ntoa(ipStruct.sin_addr);  
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
