#include "TCPSession.h"
#include <chrono>
#include <bits/this_thread_sleep.h>

double GetTimeMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now().time_since_epoch()
           ).count();
}

void TCPSession::SendAck(uint32_t ackNum)
{
    header.ack = ackNum;
    header.SetFlagACK(true);
    socket.SendPacket(TCPPacket{header, ""}, destIP);
    header.SetFlagACK(false);
}

void TCPSession::HandlePacket(const TCPPacket& packet)
{
    uint32_t packetSeq = packet.header.seq;
    uint32_t packetAck = packet.header.ack;
    uint32_t packetLen = packet.data.size();

    if (packet.header.GetFlagACK())
    {
        for (auto it = unackedPackets.begin(); it != unackedPackets.end(); ) {
            if (it->first < packetAck)
                it = unackedPackets.erase(it);
            else
                ++it;
        }
        return;
    }

    if (packetSeq == expectedSeq) {
        OnDataReceived(packet.data);
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        SendAck(expectedSeq);
        expectedSeq += packetLen;

        while (packetBuffer.count(expectedSeq)) {
            TCPPacket nextPacket = packetBuffer[expectedSeq];
            OnDataReceived(nextPacket.data);
            packetBuffer.erase(expectedSeq);
            expectedSeq += nextPacket.data.size();
        }
    }
    else if (packetSeq > expectedSeq) {
        packetBuffer[packetSeq] = packet;
        SendAck(expectedSeq);
    }
    else
        SendAck(expectedSeq);
}

void TCPSession::RetransmitIfTimeout()
{
    double now = GetTimeMillis();

    for (auto& [seq, packetInfo] : unackedPackets) {
        if (now - packetInfo.timestamp > retransmitTimeoutMs) {
            socket.SendPacket(packetInfo.packet, destIP);
            packetInfo.timestamp = now;
        }
    }
}

void TCPSession::SendData(const string& data)
{
    header.seq = mySeq;
    socket.SendPacket({header, data}, destIP);
    mySeq += data.size();
}

void TCPSession::Finish()
{
    header.seq = mySeq;
    header.SetFlagFIN(true);
    socket.SendPacket({header, ""}, destIP);
}

bool TCPSession::IsConnected()
{
    return connected;
}

TCPSession::TCPSession(): port(NetworkNumber<Port>(-1, NumberType::Host)) {}