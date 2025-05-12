#include "Socket.h"
#include "Application.h"
#include "Utils.h"

Socket::Socket()
{
    //Create the socket
    socket = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (socket < 0)
        Application::Log("Socket creation failed: " + Utils::GetSocketErrorString());

    //Set socket option to not automatically add tpc/ip header data
    int opt = 1;
    setsockopt(socket, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));

    Application::Log("Socket created");
}

int Socket::Bind(NetworkNumber<Port> port)
{
    IPAddress address = IPAddress("0.0.0.0", port);
    sockaddr_in addr = address.GetAsNetworkStruct();

    //Binding socket to pc's ip address and port
    int binderr = bind(socket, (sockaddr*)&addr, sizeof(addr));
    if (binderr < 0)
    {
        Application::Log("Socket did not bind to PC on port " + to_string(port.GetAsHost()) + " because: " + Utils::GetSocketErrorString());
        return 1;
    }
    Application::Log("Socket bound to PC successfully on port " + to_string(port.GetAsHost()));

    return 0;
}

int Socket::SendData(std::string data, IPAddress destIP)
{
    size_t dataLength = data.length();

    //Create IP header
    IPAddress localIP = Utils::GetLocalIP();
    IPHeader ipHeader = Utils::CreateIPHeader(localIP, destIP, dataLength);

    //Calculate TCP header checksum and convert to network
    tcpHeader.check = Utils::CalculateTCPChecksum(&tcpHeader, sizeof(TCPHeader) + dataLength, localIP, destIP);
    tcpHeader.ConvertToNetworkOrder();

    // Create a buffer to hold the entire packet (IP header + TCP header + data)
    size_t packetSize = sizeof(IPHeader) + sizeof(TCPHeader) + dataLength;
    char* packet = new char[packetSize];

    // Copy IP header into the packet
    memcpy(packet, &ipHeader, sizeof(IPHeader));

    // Copy TCP header after the IP header
    memcpy(Utils::AddToPointer(packet, sizeof(IPHeader)), &tcpHeader, sizeof(TCPHeader));

    // Copy the data after the TCP header
    memcpy(Utils::AddToPointer(packet, sizeof(IPHeader) + sizeof(TCPHeader)), &data[0], dataLength);

    //Convert TCP header back to host
    tcpHeader.ConvertToHostOrder();

    //Send data and check for errors
    sockaddr_in addr = destIP.GetAsNetworkStruct();
    int bytesSent = sendto(socket, packet, packetSize, 0, (sockaddr*)&addr, sizeof(sockaddr_in));

    if (bytesSent > 0)
        Application::Log("Sent packet: " + Utils::PacketToString(ipHeader, tcpHeader, data) + " " + to_string(bytesSent) + " bytes");
    else if (bytesSent < 0)
    {
        Application::Log("Sending failed: " + Utils::GetSocketErrorString());
        return 1;
    }

    tcpHeader.seq = htonl(tcpHeader.seq + dataLength != 0 ? dataLength : 1);

    return 0;
}

int Socket::ReceiveData(string* receivedData, IPAddress* senderIP, bool receiveFromAll)
{
    NetworkNumber<Port> receivedTCPHeaderDestPort = NetworkNumber<Port>(-1, NumberType::Host);
    while (receivedTCPHeaderDestPort.GetAsHost() != tcpHeader.source)
    {
        char buffer[65536];
        int bytesReceived;

        sockaddr_in senderAddr;
        if (receiveFromAll)
        {
            socklen_t senderAddrSize = sizeof(senderAddr);
            bytesReceived = recvfrom(socket, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &senderAddrSize);
        }else
        {
            senderAddr = senderIP->GetAsNetworkStruct();
            socklen_t serverAddrSize = sizeof(senderAddr);
            bytesReceived = recvfrom(socket, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &serverAddrSize);
        }

        if (bytesReceived > 0)
        {
            //Get IPHeader from the packet
            IPHeader receivedIPHeader = *(IPHeader*)&buffer;

            //Get TCPHeader from the packet
            TCPHeader receivedTCPHeader = *(TCPHeader*)Utils::AddToPointer(&buffer, sizeof(IPHeader));
            receivedTCPHeader.ConvertToHostOrder();
            receivedTCPHeaderDestPort = NetworkNumber(receivedTCPHeader.dest, NumberType::Host);

            //Check if the received packet was sent to our port
            if (receivedTCPHeaderDestPort.GetAsHost() != tcpHeader.source)
                continue;

            //Fill up senderIP to return to caller
            if (receiveFromAll)
            {
                senderAddr.sin_port = receivedTCPHeaderDestPort.GetAsNetwork();
                *senderIP = IPAddress(senderAddr);
            }

            //Get data from packet
            char* data = (char*)Utils::AddToPointer(&buffer, sizeof(IPHeader) + sizeof(TCPHeader));
            int dataLength = bytesReceived - sizeof(IPHeader) - sizeof(TCPHeader);
            string dataString(data, dataLength);

            Application::Log("Received packet: " + Utils::PacketToString(receivedIPHeader, receivedTCPHeader, dataString) + " " + to_string(bytesReceived) + " bytes");
            // if (tcpHeader.ack != receivedTcpHeader.seq)
            // {
            //     Application::Log("Receiving failed: sequence number does not match acknowledgment number", isServer);
            //     return 1;
            // }

            *receivedData = dataString;

            receivedTCPHeader.ack = htonl(ntohl(receivedTCPHeader.seq) + (dataLength != 0 ? dataLength : 1));
        }
        else if (bytesReceived < 0)
        {
            Application::Log("Receiving failed");
            return 1;
        }
    }

    return 0;
}

void Socket::Close()
{
    close(socket);
}

bool Socket::IsSocketValid()
{
    return socket < 0;
}


int Socket::Connect(IPAddress IP)
{
    sockaddr_in addr = IP.GetAsNetworkStruct();

    int err = connect(socket, (sockaddr*)&addr, sizeof(addr));
    if (err != 0)
    {
        Application::Log("Connection error: " + Utils::GetSocketErrorString());
        return 1;
    }
}

TCPHeader& Socket::GetTCPHeader()
{
    return tcpHeader;
}