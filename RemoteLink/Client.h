#pragma once
#include <string>

namespace Client
{
    int Setup();
    int Connect(std::string address, int port);
    void Update();
    bool IsConnected();
    void Disconnect();
}
