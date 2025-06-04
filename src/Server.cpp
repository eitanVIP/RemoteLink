#include "Server.h"

#include <cmath>
#include <string>
#include <thread>
#include "Application.h"
#include "Image.h"
#include "NetworkNumber.h"
#include "TCPHeader.h"
#include "TCPSession.h"
#include "Utils.h"

using namespace std;

int Server::Start(NetworkNumber<Port> port)
{
    socket.CreateSocket();
    socket.Bind(port);
    this->port = port;

    Utils::CreateInitialTCPHeader(header, port, NetworkNumber<Port>(0, NumberType::Host));

    TCPPacket packet;
    packet.header.SetFlagSYN(false);
    while (!packet.header.GetFlagSYN())
    {
        if (socket.ReceivePacket(&packet, &destIP, port) != 0)
        {
            Application::Log("Connection handshake failed at step 1");
            return 1;
        }
        if (!packet.header.GetFlagSYN())
            Application::Log("Connection handshake failed at step 1: expected SYN");
    }
    header.dest = destIP.GetPort().GetAsHost();

    requested = true;
    Application::Log("Received Connection request");
    return 0;
}

bool Server::IsRequested(IPAddress* client)
{
    if (requested)
        *client = destIP;

    return requested;
}

int Server::AcceptConnection()
{
    if (!requested)
        return 1;

    header.SetFlagACK(true);
    if (socket.SendPacket({header, ""}, destIP) != 0)
    {
        Application::Log("Connection handshake failed at step 2");
        return 1;
    }

    TCPPacket packet;
    IPAddress addr;
    if (socket.ReceivePacket(&packet, &addr, port) != 0)
    {
        Application::Log("Connection handshake failed at step 3");
        return 1;
    }

    header.SetFlagSYN(false);
    header.SetFlagACK(false);

    connected = true;
    requested = false;

    Application::Log("Connection established");

    socket.SetBlockingMode(false);
    thread serverUpdate([this]
    {
        while (Update() == 0);
    });
    serverUpdate.detach();

    return 0;
}

int Server::Update()
{
    TCPPacket packet;
    IPAddress addr;
    if (socket.ReceivePacket(&packet, &addr, port) == 0)
        HandlePacket(packet);

    RetransmitIfTimeout();

    Image screenshot;
    Utils::TakeScreenshot(screenshot, 100);

    const int bytesPerChunk = 1500;
    const int pixelsPerChunk = bytesPerChunk / 4;
    int pixels = screenshot.Size();
    int chunks = ceil(pixels / static_cast<double>(pixelsPerChunk));

    SendData("W" + to_string(screenshot.GetWidth()));
    SendData("H" + to_string(screenshot.GetHeight()));
    Application::Log(to_string(screenshot.GetValues().size()));
    Application::Log(to_string(screenshot.GetWidth() * screenshot.GetHeight() * 4));
    string totalData = screenshot.GetAsString();
    for (int i = 0; i < chunks; ++i)
    {
        SendData(totalData.substr(i * bytesPerChunk, min(bytesPerChunk, pixels * 4 - i * bytesPerChunk)));
    }
    return 1;

    return 0;
}

void Server::OnDataReceived(string data)
{
    Application::Log(data);
}

void Server::Close()
{
    requested = false;
    connected = false;
    socket.Close();
}