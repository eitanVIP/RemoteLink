#include "TCPNetwork.h"
#include <winsock2.h>
#include "Application.h"
#include "IPHeader.h"
#include "TCPHeader.h"
#include "Utils.h"

using namespace std;

namespace TCPNetwork
{
    void CreateIPHeader(IPHeader &ipHeader, uint32_t srcIP, uint32_t destIP, uint16_t payloadLength)
    {
        ipHeader.version_ihl = (4 << 4) | 5;                       // Version 4, IHL 5 (20 bytes)
        ipHeader.tos = 0;                                          // Default TOS
        ipHeader.total_length = htons(20 + payloadLength); // Header (20 bytes) + payload
        ipHeader.identification = htons(54321);            // Arbitrary ID
        ipHeader.flags_offset = htons(0x4000);             // Don't fragment
        ipHeader.ttl = 64;                                         // Common TTL value
        ipHeader.protocol = 6;                                     // TCP protocol
        ipHeader.checksum = 0;                                     // Checksum initially 0 for calculation
        ipHeader.src_ip = srcIP;                                   // Source IP in network byte order
        ipHeader.dest_ip = destIP;    
    }

    void CreateInitialTCPHeader(TCPHeader *header, uint16_t port)
    {
        //Set the first message tcp header
        header->source = port;
        header->dest = port;
        header->seq = 0;
        header->ack = 0;
        header->flags = 0;
        //SYN is set because first message
        header->SetFlagSYN(true);
        //Max 16-bit number for maximum messages size
        header->window = 0xFFFF;
    }

    void UpdateTCPHeader(TCPHeader* header)
    {
        
    }
    
    int SendData(SOCKET sock, string data, TCPHeader* tcpHeader)
    {
        size_t dataLength = sizeof(data);

        IPHeader ipHeader;
        CreateIPHeader(&ipHeader, ?, ?, ?);
        
        // Create a buffer to hold the entire packet (IP header + TCP header + data)
        int packetSize = sizeof(IPHeader) + sizeof(TCPHeader) + dataLength;
        char *packet = new char[packetSize];
        
        // Copy IP header into the packet
        memcpy(packet, &ipHeader, sizeof(IPHeader));

        // Copy TCP header into the packet (after the IP header)
        memcpy(packet + sizeof(IPHeader), tcpHeader, sizeof(TCPHeader));

        // Copy the data after the TCP header
        memcpy(packet + sizeof(IPHeader) + sizeof(TCPHeader), &data[0], dataLength);
        
        int err = send(sock, packet, packetSize, 0);
        if (err > 0)
            Application::Log("[Client] " + std::to_string(err) + " bytes sent: " + data);
        else if (err == SOCKET_ERROR)
        {
            Application::Log("[Client] Sending failed: " + Utils::GetWSAErrorString());
            return 1;
        }

        return 0;
    }
}
