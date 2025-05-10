#pragma once

#include "IPAddress.h"

namespace Client
{
    int Connect(IPAddress address, int port);
    int Update();
    bool IsConnected();
    int SendMessageToServer(string message);
    void Disconnect();
}
