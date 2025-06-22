#include "TCPSession.h"
#include <chrono>
#include <bits/this_thread_sleep.h>
#include "Application.h"

double TCPSession::GetTimeMillis() {
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
        Application::Log("Received ack until byte " + to_string(packetAck));
        return;
    }

    if (packetSeq == expectedSeq) {
        OnDataReceived(packet.data);
        expectedSeq += packetLen;

        while (packetBuffer.count(expectedSeq)) {
            TCPPacket nextPacket = packetBuffer[expectedSeq];
            OnDataReceived(nextPacket.data);
            packetBuffer.erase(expectedSeq);
            expectedSeq += nextPacket.data.size();
        }

        SendAck(expectedSeq);
    }
    else if (packetSeq > expectedSeq) {
        packetBuffer[packetSeq] = packet;
        SendAck(expectedSeq);
        Application::Log("Received out of order packet with seq " + to_string(packetSeq) + ", storing packet in buffer");
    }
    else
    {
        SendAck(expectedSeq);
        Application::Log("Received out of order packet with seq " + to_string(packetSeq) + ", ignoring packet");
    }
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
    TCPPacket packet = {header, data};
    socket.SendPacket(packet, destIP);
    unackedPackets[mySeq] = {packet, GetTimeMillis()};
    mySeq += data.size();
}

void TCPSession::Finish()
{
    header.seq = mySeq;
    header.SetFlagFIN(true);
    socket.SendPacket({header, ""}, destIP);
    socket.Close();
}

bool TCPSession::IsConnected()
{
    return connected;
}

TCPSession::TCPSession(): port(NetworkNumber<Port>(-1, NumberType::Host)) {}