#pragma once
#include "IPAddress.h"

namespace Server
{
    int Start(int port);
    bool IsRequested(IPAddress* client);
    int AcceptConnection();
    bool IsConnected();
    int Update();
    void Close();
}
