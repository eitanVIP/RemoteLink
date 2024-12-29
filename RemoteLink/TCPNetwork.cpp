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
    IPHeader CreateIPHeader(IPAddress srcIP, IPAddress destIP, uint16_t dataLength)
    {
        IPHeader header;

        // Version 4, IHL 5 (20 bytes)
        header.version_ihl = (4 << 4) | 5;
        // Default TOS
        header.tos = 0;
        // TCPHeader + data
        header.total_length = htons(sizeof(TCPHeader) + dataLength);
        // Arbitrary ID
        header.identification = htons(54321);
        // Don't fragment
        header.flags_offset = htons(0x4000);
        // Common TTL value
        header.ttl = 64;
        // TCP protocol
        header.protocol = 6;
        // Checksum initially 0 for calculation
        header.checksum = 0;
        // Network byte order
        header.src_ip = srcIP.GetAsNetworkBinary();
        // Network byte order
        header.dest_ip = destIP.GetAsNetworkBinary();

        return header;
    }

    TCPHeader CreateInitialTCPHeader(int port)
    {
        TCPHeader header;
        
        //Set the first message tcp header
        header.source = htons(port);
        header.dest = htons(port);
        header.seq = 0;
        header.ack = 0;
        header.flags = 0;
        //SYN is set because first message
        header.SetFlagSYN(true);
        //Max 16-bit number for maximum messages size
        header.window = 0xFFFF;

        return header;
    }
    
    int SendData(SOCKET sock, string data, TCPHeader& tcpHeader, IPAddress destIP, BOOL isServer)
    {
        size_t dataLength = data.length();

        //Create IP Header
        IPHeader ipHeader = CreateIPHeader(Utils::GetLocalIP(), destIP, dataLength);
        
        // Create a buffer to hold the entire packet (IP header + TCP header + data)
        int packetSize = sizeof(IPHeader) + sizeof(TCPHeader) + dataLength;
        char *packet = new char[packetSize];
        
        // Copy IP header into the packet
        memcpy(packet, &ipHeader, sizeof(IPHeader));

        // Copy TCP header into the packet (after the IP header)
        memcpy(packet + sizeof(IPHeader), &tcpHeader, sizeof(TCPHeader));

        // Copy the data after the TCP header
        memcpy(packet + sizeof(IPHeader) + sizeof(TCPHeader), &data[0], dataLength);

        //Send data and check for errors
        int err = send(sock, packet, packetSize, 0);
        //If err is bigger than 0 then it's equal to the amount of bytes sent
        if (err > 0)
            Application::Log(std::to_string(err) + " bytes sent: " + data, isServer);
        else if (err == SOCKET_ERROR)
        {
            Application::Log("Sending failed: " + Utils::GetWSAErrorString(), isServer);
            return 1;
        }

        tcpHeader.seq += (dataLength != 0 ? dataLength : 1);

        return 0;
    }

    int RecieveData(SOCKET sock, TCPHeader& tcpHeader, BOOL isServer, string& recievedData, IPAddress& senderIP, int& senderPort)
    {
        char buffer[1024];
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        
        //If error occured, bytesReceived is equal to error
        if (bytesReceived > 0)
        {
            Application::Log(to_string(bytesReceived) + " bytes received: " + buffer, isServer);

            IPHeader receivedIPHeader = *(IPHeader*)&buffer;
            TCPHeader receivedTcpHeader = *(TCPHeader*)(&buffer + sizeof(IPHeader));
            char* data = (char*)(&buffer + sizeof(IPHeader) + sizeof(TCPHeader));
            int dataLength = bytesReceived - sizeof(IPHeader) - sizeof(TCPHeader);
            string dataString(data, dataLength);

            if (tcpHeader.ack != receivedTcpHeader.seq)
            {
                Application::Log("Error receiving data- sequence number does not match acknowledgment number", isServer);
                return 1;
            }

            recievedData = dataString;
            senderIP = IPAddress(receivedIPHeader.src_ip);
            senderPort = ntohs(receivedTcpHeader.source);

            tcpHeader.SetFlagACK(true);
            tcpHeader.ack = receivedTcpHeader.seq + (dataLength != 0 ? dataLength : 1);
        }
        else if (bytesReceived == SOCKET_ERROR)
        {
            Application::Log("Receiving failed: " + Utils::GetWSAErrorString(), isServer);
            return 1;
        }

        return 0;
    }

    int ClientConnectionDance(SOCKET sock, TCPHeader& tcpHeader, IPAddress destIP, int port)
    {
        tcpHeader = CreateInitialTCPHeader(port);
        
        if (SendData(sock, "", tcpHeader, destIP, FALSE) != 0)
        {
            Application::Log("Connection dance failed at step 1", FALSE);
            return 1;
        }

        string data;
        IPAddress addr;
        int port;
        if (RecieveData(sock, tcpHeader, FALSE, data, addr, port) != 0)
        {
            Application::Log("Connection dance failed at step 2", FALSE);
            return 1;
        }
        
        if (SendData(sock, "", tcpHeader, destIP, FALSE) != 0)
        {
            Application::Log("Connection dance failed at step 3", FALSE);
            return 1;
        }
        
        tcpHeader.SetFlagSYN(false);
        tcpHeader.SetFlagACK(false);

        return 0;
    }

    int ServerConnectionDance(SOCKET sock, TCPHeader& tcpHeader, int myPort)
    {
        tcpHeader = CreateInitialTCPHeader(myPort);

        string data;
        IPAddress addr;
        int port;
        if (RecieveData(sock, tcpHeader, TRUE, data, addr, port) != 0)
        {
            Application::Log("Connection dance failed at step 1", TRUE);
            return 1;
        }
        
        if (SendData(sock, "", tcpHeader, addr, TRUE) != 0)
        {
            Application::Log("Connection dance failed at step 2", TRUE);
            return 1;
        }
        
        if (RecieveData(sock, tcpHeader, TRUE, data, addr, port) != 0)
        {
            Application::Log("Connection dance failed at step 3", TRUE);
            return 1;
        }
        
        tcpHeader.SetFlagSYN(false);
        tcpHeader.SetFlagACK(false);

        return 0;
    }
}
