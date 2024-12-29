#pragma once

#include <string>
#include <winsock2.h>
#include "IPAddress.h"

using namespace std;

namespace Utils
{
    string GetWSAErrorString();
    int SetupWSA();
    int CreateSocket(SOCKET &sock, bool isServer);
    // sockaddr_in StringToAddress(string address, int port);
    IPAddress GetLocalIP();
}
