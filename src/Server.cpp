#include "Server.h"
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "Application.h"
#include "NetworkNumber.h"
#include "TCPHeader.h"
#include "Utils.h"

using namespace std;

namespace Server
{
    Socket socket;
    IPAddress clientAddress;
    
    bool connected = false;
    bool requested = false;

    mutex mtx;
    queue<string> dataQueue;

    int Start(NetworkNumber<Port> port)
    {
        socket.CreateSocket();
        socket.Bind(port);

        Utils::CreateInitialTCPHeader(socket.GetTCPHeader(), port, NetworkNumber<Port>(0, NumberType::Host));

        string data;
        IPAddress clientAddress;
        if (socket.ReceiveData(&data, &clientAddress, true) != 0)
        {
            Application::Log("Connection handshake failed at step 1");
            return 1;
        }
        socket.GetTCPHeader().dest = clientAddress.GetPort().GetAsHost();

        requested = true;
        Application::Log("Received Connection request");
        return 0;
    }

    bool IsRequested(IPAddress* client)
    {
        if (requested)
            *client = clientAddress;

        return requested;
    }

    int AcceptConnection()
    {
        if (!requested)
            return 1;


        if (socket.SendData("", clientAddress) != 0)
        {
            Application::Log("Connection handshake failed at step 2");
            return 1;
        }

        string data;
        if (socket.ReceiveData(&data, &clientAddress, false) != 0)
        {
            Application::Log("Connection handshake failed at step 3");
            return 1;
        }

        socket.GetTCPHeader().SetFlagSYN(false);
        socket.GetTCPHeader().SetFlagACK(false);

        connected = true;
        requested = false;
        
        Application::Log("Connection established");

        thread serverUpdate([]
        {
            while (Update() == 0);
        });
        serverUpdate.detach();
        
        return 0;
    }

    bool IsConnected()
    {
        return connected;
    }

    int Update() {
        string data;
        return socket.ReceiveData(&data, &clientAddress, false);
    }

    void Close()
    {
        requested = false;
        connected = false;
        socket.Close();
    }
}
