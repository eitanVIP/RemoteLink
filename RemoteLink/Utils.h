#pragma once

#include <string>
#include <winsock2.h>
#include "IPAddress.h"
#include "IPHeader.h"
#include "TCPHeader.h"
#include <sstream>

namespace Utils
{
    string GetWSAErrorString();
    int SetupWSA();
    int CreateSocket(SOCKET* sock, BOOL isServer);
    // sockaddr_in StringToAddress(string address, int port);
    IPAddress GetLocalIP();

    template<typename T>
    T* AddToPointer(T* pointer, size_t offset)
    {
        return reinterpret_cast<T*>(reinterpret_cast<char*>(pointer) + offset);
    }

    std::string PacketToString(IPHeader ipHeader, TCPHeader tcpHeader, string data);
}
