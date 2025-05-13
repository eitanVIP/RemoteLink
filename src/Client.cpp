#include "Client.h"
#include <string>
#include "Application.h"
#include "Socket.h"
#include "TCPHeader.h"
#include "Utils.h"

using namespace std;

namespace Client
{
    bool connected = false;
    Socket socket;
    IPAddress serverAddress;
    
    int Connect(IPAddress address)
    {
        serverAddress = address;

        socket.CreateSocket();
        socket.Connect(address);

        Utils::CreateInitialTCPHeader(socket.GetTCPHeader(), NetworkNumber<Port>(Utils::GetRandomPort(), NumberType::Host), address.GetPort());

        if (socket.SendData("", address) != 0)
        {
            Application::Log("Connection handshake failed at step 1");
            return 1;
        }

        string data;
        if (socket.ReceiveData(&data, &address, false) != 0)
        {
            Application::Log("Connection handshake failed at step 2");
            return 1;
        }

        socket.GetTCPHeader().SetFlagSYN(false);
        socket.GetTCPHeader().SetFlagACK(true);
        if (socket.SendData("", address) != 0)
        {
            Application::Log("Connection handshake failed at step 3");
            return 1;
        }
        socket.GetTCPHeader().SetFlagACK(false);

        connected = true;
        Application::Log("Connection established");
        return 0;
    }

    int Update()
    {
        return 0;
    }

    bool IsConnected()
    {
        return connected;
    }

    int SendMessageToServer(string message)
    {
        return socket.SendData(message, serverAddress);
    }

    void Disconnect()
    {
        connected = false;
        socket.Close();
    }
}
