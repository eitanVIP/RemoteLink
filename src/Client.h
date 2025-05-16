#pragma once

#include "IPAddress.h"
#include "TCPSession.h"

class Client : public TCPSession
{
public:
    int Connect(IPAddress address);
    int Update();
    int SendMessageToServer(string message);
    void Disconnect();
    void OnDataReceived(string data) override;
};
