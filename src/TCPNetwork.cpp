#include "TCPNetwork.h"
#include "Application.h"
#include "IPAddress.h"
#include "IPHeader.h"
#include "TCPHeader.h"
#include "Utils.h"
#include <cmath>

using namespace std;

namespace TCPNetwork
{
    int ServerHandshakeStep1(int sock, TCPHeader& tcpHeader, NetworkNumber<unsigned short> serverPort, IPAddress* clientAddress)
    {
        CreateInitialTCPHeader(tcpHeader, serverPort, *new NetworkNumber<unsigned short>(0, NumberType::Host));

        string data;
        if (ReceiveData(sock, tcpHeader, &data, clientAddress, true) != 0)
        {
            Application::Log("Connection handshake failed at step 1", true);
            return 1;
        }
        tcpHeader.dest = clientAddress->GetPort().GetAsNetwork();

        return 0;
    }

    int ServerHandshakeStep2(int sock, TCPHeader& tcpHeader, IPAddress clientAddress)
    {
        if (SendData(sock, "", tcpHeader, clientAddress, true) != 0)
        {
            Application::Log("Connection handshake failed at step 2", true);
            return 1;
        }

        string data;
        if (ReceiveData(sock, tcpHeader, &data, &clientAddress, true) != 0)
        {
            Application::Log("Connection handshake failed at step 3", true);
            return 1;
        }
        
        tcpHeader.SetFlagSYN(false);
        tcpHeader.SetFlagACK(false);

        return 0;
    }
}
