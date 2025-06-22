#pragma once

#include <queue>

#include "Image.h"
#include "IPAddress.h"
#include "TCPSession.h"

class Client : public TCPSession
{
private:
    int expectedImageWidth = 0;
    int expectedImageHeight = 0;
    int expectedImageSize = 0;
    int imageBytesReceived = 0;
    string imageData;
    std::queue<Image> images;

public:
    int Connect(IPAddress address);
    int Update();
    int SendMessageToServer(string message);
    void OnFinish() override;
    void OnDataReceived(string data) override;
    std::queue<Image>& GetImages();
};
