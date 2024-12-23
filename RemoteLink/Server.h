#pragma once

namespace Server
{
    int Start(int port);
    void Update();
    bool IsConnected();
    bool IsInitialized();
    void Close();
}