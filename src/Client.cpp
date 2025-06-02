#include "Client.h"
#include <string>
#include <thread>

#include "Application.h"
#include "Socket.h"
#include "Utils.h"

using namespace std;

int Client::Connect(IPAddress address)
{
    destIP = address;

    socket.CreateSocket();
    socket.Connect(address);

    this->port = NetworkNumber<Port>(Utils::GetRandomPort(), NumberType::Host);

    Utils::CreateInitialTCPHeader(header, port, address.GetPort());

    if (socket.SendPacket({header, ""}, destIP) != 0)
    {
        Application::Log("Connection handshake failed at step 1");
        return 1;
    }

    TCPPacket packet;
    while (!packet.header.GetFlagSYN())
    {
        if (socket.ReceivePacket(&packet, &destIP, port) != 0)
        {
            Application::Log("Connection handshake failed at step 2");
            return 1;
        }
        if (!packet.header.GetFlagSYN())
            Application::Log("Connection handshake failed at step 1: expected SYN");
    }

    header.SetFlagSYN(false);
    header.SetFlagACK(true);
    if (socket.SendPacket({header, ""}, destIP) != 0)
    {
        Application::Log("Connection handshake failed at step 3");
        return 1;
    }
    header.SetFlagACK(false);

    connected = true;
    Application::Log("Connection established");

    socket.SetBlockingMode(false);
    thread clientUpdate([this]
    {
        while (Update() == 0);
    });
    clientUpdate.detach();

    return 0;
}

int Client::Update()
{
    TCPPacket packet;
    IPAddress addr;
    if (socket.ReceivePacket(&packet, &addr, port) == 0)
        HandlePacket(packet);

    RetransmitIfTimeout();

    return 0;
}

int Client::SendMessageToServer(string message)
{
    SendData(message);

    return 0;
}

void Client::Disconnect()
{
    connected = false;
    socket.Close();
}

void Client::OnDataReceived(string data)
{
    for (int i = 0; i < data.size(); i += 4)
    {
        uint8_t r = data[i];
        uint8_t g = data[i + 1];
        uint8_t b = data[i + 2];
        uint8_t a = data[i + 3];
        Application::Log(to_string(r) + ", " + to_string(g) + ", " + to_string(b) + ", " + to_string(a));
    }
}