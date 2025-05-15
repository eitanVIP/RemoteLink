#include "Socket.h"
#include <unistd.h>
#include "Application.h"
#include "Utils.h"
#include <cmath>
#include <sstream>
#include <ifaddrs.h>

int Socket::CreateSocket()
{
    //Create the socket
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0)
    {
        Application::Log("Socket creation failed: " + Utils::GetSocketErrorString());
        return 1;
    }

    //Set socket option to not automatically add tpc/ip header data
    int opt = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));

    Application::Log("Socket created");
    return 0;
}

int Socket::Bind(NetworkNumber<Port> port)
{
    IPAddress address = IPAddress("0.0.0.0", port);
    sockaddr_in addr = address.GetAsNetworkStruct();

    //Binding socket to pc's ip address and port
    int binderr = bind(sock, (sockaddr*)&addr, sizeof(addr));
    if (binderr < 0)
    {
        Application::Log("Socket did not bind to PC on port " + to_string(port.GetAsHost()) + " because: " + Utils::GetSocketErrorString());
        return 1;
    }
    Application::Log("Socket bound to PC successfully on port " + to_string(port.GetAsHost()));

    return 0;
}

int Socket::SendPacket(TCPPacket tcpPacket, IPAddress destIP) const
{
    size_t dataLength = tcpPacket.data.length();

    //Create IP header
    // IPAddress localIP = Utils::GetLocalIP();
    IPAddress localIP = IPAddress("127.0.0.1", NetworkNumber<Port>(-1, NumberType::Host));
    IPHeader ipHeader = Utils::CreateIPHeader(localIP, destIP, dataLength);

    //Calculate TCP header checksum and convert to network
    tcpPacket.header.check = Utils::CalculateTCPChecksum(&tcpPacket.header, sizeof(TCPHeader) + dataLength, localIP, destIP);
    tcpPacket.header.ConvertToNetworkOrder();

    // Create a buffer to hold the entire packet (IP header + TCP header + data)
    size_t packetSize = sizeof(IPHeader) + sizeof(TCPHeader) + dataLength;
    char* packet = new char[packetSize];

    // Copy IP header into the packet
    memcpy(packet, &ipHeader, sizeof(IPHeader));

    // Copy TCP header after the IP header
    memcpy(Utils::AddToPointer(packet, sizeof(IPHeader)), &tcpPacket.header, sizeof(TCPHeader));

    // Copy the data after the TCP header
    memcpy(Utils::AddToPointer(packet, sizeof(IPHeader) + sizeof(TCPHeader)), &tcpPacket.data[0], dataLength);

    //Convert TCP header back to host
    tcpPacket.header.ConvertToHostOrder();

    //Send data and check for errors
    sockaddr_in addr = destIP.GetAsNetworkStruct();
    int bytesSent = sendto(sock, packet, packetSize, 0, (sockaddr*)&addr, sizeof(sockaddr_in));

    if (bytesSent < 0)
    {
        Application::Log("Sending failed: " + Utils::GetSocketErrorString());
        return 1;
    }
    Application::Log("Sent packet: " + Utils::PacketToString(ipHeader, tcpPacket.header, tcpPacket.data) + " " + to_string(bytesSent) + " bytes");

    return 0;
}

int Socket::ReceivePacket(TCPPacket* receivedPacket, IPAddress* senderIP, NetworkNumber<Port> port) const
{
    NetworkNumber<Port> receivedTCPHeaderDestPort = NetworkNumber<Port>(-1, NumberType::Host);
    while (receivedTCPHeaderDestPort.GetAsHost() != port.GetAsHost())
    {
        char buffer[65536];

        sockaddr_in senderAddr{};
        socklen_t senderAddrSize = sizeof(senderAddr);
        int bytesReceived = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &senderAddrSize);

        if (bytesReceived > 0)
        {
            //Get IPHeader from the packet
            IPHeader receivedIPHeader = *(IPHeader*)&buffer;

            //Get TCPHeader from the packet
            TCPHeader receivedTCPHeader = *(TCPHeader*)Utils::AddToPointer(&buffer, sizeof(IPHeader));
            receivedTCPHeader.ConvertToHostOrder();
            receivedTCPHeaderDestPort = NetworkNumber(receivedTCPHeader.dest, NumberType::Host);

            //Check if the received packet was sent to our port
            if (receivedTCPHeaderDestPort.GetAsHost() != port.GetAsHost())
                continue;

            //Fill up senderIP to return to caller
            senderAddr.sin_port = htons(receivedTCPHeader.source);
            *senderIP = IPAddress(senderAddr);

            //Get data from packet
            char* data = (char*)Utils::AddToPointer(&buffer, sizeof(IPHeader) + sizeof(TCPHeader));
            int dataLength = bytesReceived - sizeof(IPHeader) - sizeof(TCPHeader);
            string dataString(data, dataLength);

            *receivedPacket = {receivedTCPHeader, dataString};
            Application::Log("Received packet: " + Utils::PacketToString(receivedIPHeader, receivedTCPHeader, dataString) + " " + to_string(bytesReceived) + " bytes");
        }
        else if (bytesReceived < 0)
        {
            Application::Log("Receiving failed: " + Utils::GetSocketErrorString());
            return 1;
        }
    }

    return 0;
}

void Socket::Close()
{
    close(sock);
}

int Socket::Connect(IPAddress IP)
{
    sockaddr_in addr = IP.GetAsNetworkStruct();

    int err = connect(sock, (sockaddr*)&addr, sizeof(addr));
    if (err != 0)
    {
        Application::Log("Connection error: " + Utils::GetSocketErrorString());
        return 1;
    }

    return 0;
}
