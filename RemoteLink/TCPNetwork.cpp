#include "TCPNetwork.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Application.h"
#include "IPAddress.h"
#include "IPHeader.h"
#include "TCPHeader.h"
#include "Utils.h"

using namespace std;

namespace TCPNetwork
{
    uint16_t CalculateIPChecksum(uint16_t* data, size_t length) {
        uint32_t sum = 0;

        while (length > 1) {
            sum += *data++;
            length -= 2;
        }

        if (length == 1) { // Handle odd byte at the end
            sum += *(uint8_t*)data;
        }

        sum = (sum >> 16) + (sum & 0xFFFF); // Fold high into low
        sum += (sum >> 16);                 // Handle carry
        return (uint16_t)~sum;
    }
    
    IPHeader CreateIPHeader(IPAddress srcIP, IPAddress destIP, size_t dataLength)
    {
        IPHeader header;

        // Version 4, IHL 5 (20 bytes)
        header.version_ihl = (4 << 4) | 5;
        // Default TOS
        header.tos = 0;
        // IPHeader + TCPHeader + data
        header.total_length = htons(sizeof(IPHeader) + sizeof(TCPHeader) + dataLength);
        // Arbitrary ID
        header.identification = htons(round(10000 * rand() / (RAND_MAX + 1.0) + 5000));
        // Don't fragment
        header.flags_offset = htons(0x4000);
        // Common TTL value
        header.ttl = 64;
        // TCP protocol
        header.protocol = 6;
        // Network byte order
        header.src_ip = srcIP.GetAsNetworkBinary();
        // Network byte order
        header.dest_ip = destIP.GetAsNetworkBinary();
        // Checksum calculation
        header.checksum = 0;
        header.checksum = CalculateIPChecksum((uint16_t*)&header, sizeof(IPHeader));

        return header;
    }

    struct PseudoHeader
    {
        uint32_t srcIP;
        uint32_t destIP;
        uint8_t reserved;  // Must be 0
        uint8_t protocol;
        uint16_t tcpLength;  // TCP header + payload
    };
    
    uint16_t CalculateTCPChecksum(TCPHeader* headerPtr, size_t tcpLength, IPAddress srcIP, IPAddress destIP) {
        headerPtr->check = 0;
        
        struct PseudoHeader pheader;
        pheader.srcIP = srcIP.GetAsNetworkBinary();
        pheader.destIP = destIP.GetAsNetworkBinary();
        pheader.reserved = 0;
        pheader.protocol = 6; // TCP protocol number
        pheader.tcpLength = htons(tcpLength);

        uint32_t sum = 0;
        uint16_t* pheader_ptr = (uint16_t*)&pheader;

        // Add pseudo-header
        for (size_t i = 0; i < sizeof(PseudoHeader) / 2; ++i) {
            sum += *pheader_ptr++;
        }

        uint16_t* segmentPtr = (uint16_t*)headerPtr;
        // Add TCP segment (header + data)
        while (tcpLength > 1) {
            sum += *segmentPtr++;
            tcpLength -= 2;
        }

        if (tcpLength == 1) { // Handle odd byte
            sum += *(uint8_t*)segmentPtr;
        }

        sum = (sum >> 16) + (sum & 0xFFFF); // Fold high into low
        sum += (sum >> 16);                 // Handle carry
        return (uint16_t)~sum;
    }

    void CreateInitialTCPHeader(TCPHeader& header, int sourcePort, int destPort)
    {
        //Set the first message tcp header
        header.source = htons(sourcePort);
        header.dest = htons(destPort);
        header.seq = 0;
        header.ack = 0;
        header.flags = 0;
        //SYN is set because first message
        header.SetFlagSYN(true);
        //Max 16-bit number for maximum messages size
        header.window = 0xFFFF;
        header.check = 0;
        header.urg_ptr = 0;
    }
    
    int SendData(SOCKET sourceSocket, string data, TCPHeader& sourceTCPHeader, IPAddress destIP, int destPort, BOOL isServer)
    {
        size_t dataLength = data.length();

        //Create IP header
        IPHeader ipHeader = CreateIPHeader(Utils::GetLocalIP(), destIP, dataLength);

        //Calculate TCP header checksum
        sourceTCPHeader.check = CalculateTCPChecksum(&sourceTCPHeader, sizeof(TCPHeader) + dataLength, Utils::GetLocalIP(), destIP);
        
        // Create a buffer to hold the entire packet (IP header + TCP header + data)
        int packetSize = sizeof(IPHeader) + sizeof(TCPHeader) + dataLength;
        char* packet = new char[packetSize];
        
        // Copy IP header into the packet
        memcpy(packet, &ipHeader, sizeof(IPHeader));

        // Copy TCP header into the packet (after the IP header)
        memcpy(Utils::AddToPointer(packet, sizeof(IPHeader)), &sourceTCPHeader, sizeof(TCPHeader));

        // Copy the data after the TCP header
        memcpy(Utils::AddToPointer(packet, sizeof(IPHeader) + sizeof(TCPHeader)), &data[0], dataLength);

        // for (int i = 0; i < packetSize; i++)
        // {
        //     uint8_t byte = packet[i];
        //     char buffer[4]; // Enough to hold "FF\0"
        //     snprintf(buffer, sizeof(buffer), "%02X", byte);
        //     Application::Log(buffer, isServer);
        // }

        Application::Log("Sent packet: " + Utils::PacketToString(ipHeader, sourceTCPHeader, data));
        
        //Send data and check for errors
        sockaddr_in addr = destIP.GetAsNetworkStruct();
        addr.sin_port = htons(destPort);
        int bytesSent = sendto(sourceSocket, packet, packetSize, 0, (sockaddr*)&addr, sizeof(*(sockaddr*)&addr));
        //If error occured, bytesSent is equal to error
        if (bytesSent > 0)
            Application::Log(to_string(bytesSent) + " bytes sent", isServer);
        else if (bytesSent == SOCKET_ERROR)
        {
            Application::Log("Sending failed: " + Utils::GetWSAErrorString(), isServer);
            return 1;
        }

        sourceTCPHeader.seq += (dataLength != 0 ? dataLength : 1);

        return 0;
    }

    int ReceiveData(SOCKET receivingSocket, TCPHeader& receivingTCPHeader, string* receivedData, IPAddress* senderIP, int* senderPort, BOOL isServer)
    {
        char buffer[1024];
        int bytesReceived;
        if (isServer == TRUE)
        {
            sockaddr_in senderAddr;
            int senderAddrSize = sizeof(senderAddr);
            bytesReceived = recvfrom(receivingSocket, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &senderAddrSize);
            *senderIP = IPAddress(senderAddr);
            *senderPort = ntohs(senderAddr.sin_port);
        }else
        {
            sockaddr_in senderAddr = senderIP->GetAsNetworkStruct();
            int serverAddrSize = sizeof(senderAddr);
            bytesReceived = recvfrom(receivingSocket, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &serverAddrSize);
        }
        
        //If error occured, bytesReceived is equal to error
        if (bytesReceived > 0)
        {
            Application::Log(to_string(bytesReceived) + " bytes received", isServer);

            IPHeader receivedIPHeader = *(IPHeader*)&buffer;
            TCPHeader receivedTcpHeader = *(TCPHeader*)Utils::AddToPointer(&buffer, sizeof(IPHeader));
            char* data = (char*)Utils::AddToPointer(&buffer, sizeof(IPHeader) + sizeof(TCPHeader));
            int dataLength = bytesReceived - sizeof(IPHeader) - sizeof(TCPHeader);
            string dataString(data, dataLength);

            Application::Log("Received packet: " + Utils::PacketToString(receivedIPHeader, receivedTcpHeader, dataString), isServer);
            // if (tcpHeader.ack != receivedTcpHeader.seq)
            // {
            //     Application::Log("Receiving failed: sequence number does not match acknowledgment number", isServer);
            //     return 1;
            // }

            *receivedData = dataString;

            receivingTCPHeader.SetFlagACK(true);
            receivingTCPHeader.ack = receivedTcpHeader.seq + (dataLength != 0 ? dataLength : 1);
        }
        else if (bytesReceived == SOCKET_ERROR)
        {
            Application::Log("Receiving failed: " + Utils::GetWSAErrorString(), isServer);
            return 1;
        }

        return 0;
    }

    int ClientHandshake(SOCKET sock, TCPHeader& tcpHeader, IPAddress destIP, int destPort, int clientPort)
    {
        CreateInitialTCPHeader(tcpHeader, clientPort, destPort);
        
        if (SendData(sock, "", tcpHeader, destIP, destPort, FALSE) != 0)
        {
            Application::Log("Connection handshake failed at step 1", FALSE);
            return 1;
        }

        string data;
        if (ReceiveData(sock, tcpHeader, &data, &destIP, &destPort, FALSE) != 0)
        {
            Application::Log("Connection handshake failed at step 2", FALSE);
            return 1;
        }
        
        tcpHeader.SetFlagSYN(false);
        if (SendData(sock, "", tcpHeader, destIP, destPort, FALSE) != 0)
        {
            Application::Log("Connection handshake failed at step 3", FALSE);
            return 1;
        }
        
        tcpHeader.SetFlagACK(false);

        return 0;
    }

    int ServerHandshakeStep1(SOCKET sock, TCPHeader& tcpHeader, int serverPort, IPAddress* clientAddress, int* clientPort)
    {
        CreateInitialTCPHeader(tcpHeader, serverPort, 0);

        string data;
        if (ReceiveData(sock, tcpHeader, &data, clientAddress, clientPort, TRUE) != 0)
        {
            Application::Log("Connection handshake failed at step 1", TRUE);
            return 1;
        }
        tcpHeader.dest = *clientPort;

        return 0;
    }

    int ServerHandshakeStep2(SOCKET sock, TCPHeader& tcpHeader, IPAddress clientAddress, int clientPort)
    {
        if (SendData(sock, "", tcpHeader, clientAddress, clientPort, TRUE) != 0)
        {
            Application::Log("Connection handshake failed at step 2", TRUE);
            return 1;
        }

        string data;
        if (ReceiveData(sock, tcpHeader, &data, &clientAddress, &clientPort, TRUE) != 0)
        {
            Application::Log("Connection handshake failed at step 3", TRUE);
            return 1;
        }
        
        tcpHeader.SetFlagSYN(false);
        tcpHeader.SetFlagACK(false);

        return 0;
    }
}
