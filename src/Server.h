#pragma once
#include "IPAddress.h"

namespace Server
{
    int Start(NetworkNumber<unsigned short> port);
    bool IsRequested(IPAddress* client);
    int AcceptConnection();
    bool IsConnected();
    int Update();
    void Close();
}
