#include <map>

#include "SentPacketInfo.h"
#include "Socket.h"
#include "TCPPacket.h"

class TCPSession {
protected:
    Socket socket = {};
    IPAddress destIP;
    uint32_t mySeq = 0;
    uint32_t expectedSeq = 0;
    TCPHeader header = {};
    bool connected = false;
    NetworkNumber<Port> port;

    std::map<uint32_t, TCPPacket> packetBuffer;
    std::map<uint32_t, SentPacketInfo> unackedPackets;
    const double retransmitTimeoutMs = 1000;

    void SendAck(uint32_t ackNum);
    void HandlePacket(const TCPPacket& packet);
    void RetransmitIfTimeout();
    void SendData(const string& data);
    void Finish();
    bool IsConnected();

    virtual void OnDataReceived(string data) = 0;
};
