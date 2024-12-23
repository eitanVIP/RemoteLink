#pragma once

namespace Client
{
    int Connect(const char* hostAddr, int port);
    int Update();
    bool IsConnected();
    void Disconnect();
}