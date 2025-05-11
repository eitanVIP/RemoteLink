#pragma once

#include "IPAddress.h"

namespace Client
{
    int Connect(IPAddress address);
    int Update();
    bool IsConnected();
    int SendMessageToServer(string message);
    void Disconnect();
}
