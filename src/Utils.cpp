#include "Utils.h"
#include <string>
#include "Application.h"
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <string>
#include <arpa/inet.h>

namespace Utils
{
    // std::string GetWSAErrorString() {
    //     char* messageBuffer = nullptr;
    //
    //     // Use FormatMessage to get the error string
    //     DWORD size = FormatMessageA(
    //         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    //         nullptr,
    //         WSAGetLastError(),
    //         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    //         reinterpret_cast<LPSTR>(&messageBuffer),
    //         0,
    //         nullptr);
    //
    //     std::string errorMessage;
    //
    //     if (size > 0 && messageBuffer != nullptr) {
    //         errorMessage = messageBuffer;
    //         // Free the buffer allocated by FormatMessage
    //         LocalFree(messageBuffer);
    //     } else {
    //         errorMessage = "Unknown error code: " + std::to_string(WSAGetLastError());
    //     }
    //
    //     return errorMessage.substr(0, errorMessage.length() - 2);
    // }
    //
    // int SetupWSA()
    // {
    //     WSADATA wsaData;
    //     int WSAerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    //     if (WSAerr != 0)
    //     {
    //         Application::Log("WSAStartup failed with error: " + GetWSAErrorString());
    //         return 1;
    //     }
    //
    //     Application::Log("WSAStartup successful");
    //     return 0;
    // }

    int CreateSocket(int* sock, bool isServer)
    {
        //Create the socket
        *sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
        if (*sock < 0)
        {
            Application::Log("Socket creation failed", isServer);
            return 1;
        }

        //Set socket to non-blocking mode - doesn't wait for data, if there isn't data it just continues the code
        // u_long mode = 1; // non-blocking mode
        // ioctlsocket(sock, FIONBIO, &mode);

        // //Set socket option to not automatically add tpc/ip header data
        // int opt = 1;
        // setsockopt(*sock, IPPROTO_IP, IP_HDRINCL, reinterpret_cast<const char*>(&opt), sizeof(opt));
        //
        // opt = 1;
        // setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
        //
        // DWORD dwValue = 1;
        // WSAIoctl(*sock, SIO_RCVALL, &dwValue, sizeof(dwValue), NULL, 0, NULL, NULL, NULL);

        Application::Log("Socket created", isServer);
        return 0;
    }

    // sockaddr_in StringToAddress(std::string address, int port)
    // {
    //     //Create wsa address struct
    //     sockaddr_in addr;
    //     //Using IPv4
    //     addr.sin_family = AF_INET;
    //     //Convert string address to network presentation
    //     inet_pton(AF_INET, address.c_str(), &addr.sin_addr.s_addr);
    //     //Convert port to network presentation
    //     addr.sin_port = htons(port);
    //
    //     return addr;
    // }

    // IPAddress GetLocalIP(bool isServer)
    // {
    //     char hostname[256];
    //
    //     // Get the hostname of the local machine
    //     if (gethostname(hostname, sizeof(hostname)) != 0) {
    //         Application::Log("Failed to get local host name");
    //         return IPAddress(0);  // Error, return invalid IP
    //     }
    //
    //     struct addrinfo hints = {};
    //     hints.ai_family = AF_INET;  // Only IPv4 addresses
    //     hints.ai_socktype = SOCK_STREAM;
    //
    //     struct addrinfo* result = nullptr;
    //     if (getaddrinfo(hostname, nullptr, &hints, &result) != 0) {
    //         Application::Log("Failed to get local IP from host name");
    //         return IPAddress(0);  // Error, return invalid IP
    //     }
    //
    //     // Traverse the addrinfo results to find a valid IPv4 address
    //     sockaddr_in* sa = nullptr;
    //     for (struct addrinfo* p = result; p != nullptr; p = p->ai_next) {
    //         if (p->ai_family == AF_INET) {  // Only process IPv4 addresses
    //             sa = reinterpret_cast<sockaddr_in*>(p->ai_addr);
    //             break;
    //         }
    //     }
    //
    //     if (sa == nullptr) {
    //         Application::Log("No valid IPv4 address found");
    //         freeaddrinfo(result);  // Clean up
    //         return IPAddress(0);  // Error, return invalid IP
    //     }
    //
    //     // Convert sockaddr_in to IPAddress
    //     IPAddress ip(*sa);
    //     freeaddrinfo(result);  // Clean up
    //
    //     Application::Log("Successfully got local IP: " + ip.GetAsString(), isServer);
    //     return ip;
    // }

    std::string PacketToString(IPHeader ipHeader, TCPHeader tcpHeader, string data)
    {
        std::stringstream ss;
    
        // Convert IP addresses from uint32_t to readable format
        uint8_t* src = reinterpret_cast<uint8_t*>(&ipHeader.src_ip);
        uint8_t* dest = reinterpret_cast<uint8_t*>(&ipHeader.dest_ip);
    
        ss << "Packet(";
    
        ss << "IP: "
           << int(src[0]) << "." << int(src[1]) << "." << int(src[2]) << "." << int(src[3])
           << " -> "
           << int(dest[0]) << "." << int(dest[1]) << "." << int(dest[2]) << "." << int(dest[3])
           << ", ";
    
        ss << "TCP: " 
           << ntohs(tcpHeader.source) << " -> " << ntohs(tcpHeader.dest)
           << ", ";
    
        ss << "SEQ: " << ntohl(tcpHeader.seq)
           << ", ACK: " << ntohl(tcpHeader.ack)
           << ", Flags: ";
    
        if (tcpHeader.GetFlagFIN()) ss << "FIN ";
        if (tcpHeader.GetFlagSYN()) ss << "SYN ";
        if (tcpHeader.GetFlagRST()) ss << "RST ";
        if (tcpHeader.GetFlagPSH()) ss << "PSH ";
        if (tcpHeader.GetFlagACK()) ss << "ACK ";
        if (tcpHeader.GetFlagURG()) ss << "URG ";

        ss << ", Data: " << data;
    
        ss << ")";
    
        return ss.str();
    }
}
