#pragma once

#include <string>
#include "IPAddress.h"
#include "TCPHeader.h"
#include "IPHeader.h"
#include "Application.h"
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>

namespace Utils
{
    std::string GetSocketErrorString();
    int CreateSocket(int* sock, bool isServer);
    IPAddress GetLocalIP(bool isServer);

    template<typename T>
    T* AddToPointer(T* pointer, size_t offset)
    {
        return reinterpret_cast<T*>(reinterpret_cast<char*>(pointer) + offset);
    }

    std::string PacketToString(IPHeader ipHeader, TCPHeader tcpHeader, string data);
}
