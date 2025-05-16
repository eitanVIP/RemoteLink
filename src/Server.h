#pragma once

#include "IPAddress.h"
#include "TCPSession.h"

class Server : public TCPSession
{
private:
    bool requested = false;

public:
    int Start(NetworkNumber<Port> port);
    bool IsRequested(IPAddress* client);
    int AcceptConnection();
    int Update();
    void Close();
    void OnDataReceived(string data) override;
};
