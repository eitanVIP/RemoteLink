#include "Utils.h"
#include <string>
#include "Application.h"
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <ifaddrs.h>

namespace Utils
{
    std::string GetSocketErrorString() {
        return strerror(errno);
    }

    int CreateSocket(int* sock, bool isServer)
    {
        //Create the socket
        *sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
        if (*sock < 0)
        {
            Application::Log("Socket creation failed: " + GetSocketErrorString(), isServer);
            return 1;
        }

        //Set socket to non-blocking mode - doesn't wait for data, if there isn't data it just continues the code
        // u_long mode = 1; // non-blocking mode
        // ioctlsocket(sock, FIONBIO, &mode);

        //Set socket option to not automatically add tpc/ip header data
        int opt = 1;
        setsockopt(*sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));

        Application::Log("Socket created", isServer);
        return 0;
    }

    IPAddress GetLocalIP(bool isServer)
    {
        struct ifaddrs *interfaces = nullptr;
        struct ifaddrs *ifa = nullptr;
        void *tmpAddrPtr = nullptr;
        string localIP = "";

        // Get the list of interfaces
        if (getifaddrs(&interfaces) == 0)
        {
            // Loop through the list of interfaces
            for (ifa = interfaces; ifa != nullptr; ifa = ifa->ifa_next)
            {
                if (ifa->ifa_addr->sa_family == AF_INET) // Check if it's an IPv4 address
                {
                    // Get the IP address of the interface
                    tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    char ipStr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, tmpAddrPtr, ipStr, INET_ADDRSTRLEN);

                    // Exclude the loopback address (127.0.0.1)
                    if (strcmp(ifa->ifa_name, "lo") != 0)
                    {
                        localIP = string(ipStr);
                        break; // Exit after finding the first non-loopback IP
                    }
                }
            }
        }
        else
        {
            Application::Log("Error getting network interfaces: " + string(strerror(errno)), isServer);
        }

        // Free the memory allocated by getifaddrs
        if (interfaces != nullptr)
        {
            freeifaddrs(interfaces);
        }

        if (localIP.empty())
        {
            Application::Log("No valid IP address found!", isServer);
        }
        else
        {
            Application::Log("Local IP address: " + localIP, isServer);
        }

        // Return the IPAddress object with the found IP address
        return IPAddress(localIP, NetworkNumber<unsigned short>(0, NumberType::Host));  // Assuming IPAddress class constructor accepts a string
    }

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
