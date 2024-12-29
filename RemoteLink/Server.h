#pragma once

namespace Server
{
    int Start(int port);
    int Update();
    bool IsConnected();
    void Close();
}