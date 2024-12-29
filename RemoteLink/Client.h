#pragma once
#include <string>

namespace Client
{
    int Setup();
    int Connect(std::string address, int port);
    int SendData(std::string data);
    int Update();
    bool IsConnected();
    void Disconnect();
}
