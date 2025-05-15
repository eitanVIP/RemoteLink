#pragma once

#include <cstdint>

struct TCPHeader
{
    uint16_t source;    // Source port
    uint16_t dest;      // Destination port
    uint32_t seq;       // Sequence number
    uint32_t ack;       // Acknowledgment number
    uint16_t flags;     // FIN, SYN, RST, PSH, ACK, URG
    uint16_t window;    // Window size
    uint16_t check;     // Checksum
    uint16_t urg_ptr;   // Urgent pointer

    void SetFlagFIN(bool val);
    void SetFlagSYN(bool val);
    void SetFlagRST(bool val);
    void SetFlagPSH(bool val);
    void SetFlagACK(bool val);
    void SetFlagURG(bool val);
    void SetDataOffset(uint8_t offsetWords = 5);

    bool GetFlagFIN() const;
    bool GetFlagSYN() const;
    bool GetFlagRST() const;
    bool GetFlagPSH() const;
    bool GetFlagACK() const;
    bool GetFlagURG() const;
    uint8_t GetDataOffset() const;

    void ConvertToNetworkOrder();
    void ConvertToHostOrder();
};
