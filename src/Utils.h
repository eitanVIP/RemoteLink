#pragma once

#include <string>
#include "IPAddress.h"
#include "TCPHeader.h"
#include "IPHeader.h"
#include <cstring>

#include "Image.h"
#include "Socket.h"

namespace Utils
{
    std::string GetSocketErrorString();
    IPAddress GetLocalIP();

    template<typename T>
    T* AddToPointer(T* pointer, size_t offset)
    {
        return reinterpret_cast<T*>(reinterpret_cast<char*>(pointer) + offset);
    }

    std::string PacketToString(IPHeader ipHeader, TCPHeader tcpHeader, string data);
    uint16_t CalculateIPChecksum(uint16_t* data, size_t length);
    IPHeader CreateIPHeader(IPAddress srcIP, IPAddress destIP, size_t dataLength);
    uint16_t CalculateTCPChecksum(TCPHeader* headerPtr, size_t tcpLength, IPAddress srcIP, IPAddress destIP);
    void CreateInitialTCPHeader(TCPHeader& header, NetworkNumber<Port> sourcePort, NetworkNumber<Port> destPort);
    int GetRandomPort();
    int TakeScreenshot(Image& pixels, int targetHeight);
    std::string IntToHexString(int value);
    int HexStringToInt(const std::string& hexStr);
}
