#include "TCPHeader.h"
#include <netinet/in.h>

void TCPHeader::SetFlagFIN(bool val) { 
    if (val) flags |= (1 << 0);
    else flags &= ~(1 << 0);
}

void TCPHeader::SetFlagSYN(bool val) { 
    if (val) flags |= (1 << 1);
    else flags &= ~(1 << 1);
}

void TCPHeader::SetFlagRST(bool val) { 
    if (val) flags |= (1 << 2);
    else flags &= ~(1 << 2);
}

void TCPHeader::SetFlagPSH(bool val) { 
    if (val) flags |= (1 << 3);
    else flags &= ~(1 << 3);
}

void TCPHeader::SetFlagACK(bool val) { 
    if (val) flags |= (1 << 4);
    else flags &= ~(1 << 4);
}

void TCPHeader::SetFlagURG(bool val) { 
    if (val) flags |= (1 << 5);
    else flags &= ~(1 << 5);
}

void TCPHeader::SetDataOffset(uint8_t offsetWords) {
    flags &= 0x0FFF;
    flags |= ((offsetWords & 0x0F) << 12);
}

bool TCPHeader::GetFlagFIN() const {
    return (flags & (1 << 0)) != 0;
}

bool TCPHeader::GetFlagSYN() const {
    return (flags & (1 << 1)) != 0;
}

bool TCPHeader::GetFlagRST() const {
    return (flags & (1 << 2)) != 0;
}

bool TCPHeader::GetFlagPSH() const {
    return (flags & (1 << 3)) != 0;
}

bool TCPHeader::GetFlagACK() const {
    return (flags & (1 << 4)) != 0;
}

bool TCPHeader::GetFlagURG() const {
    return (flags & (1 << 5)) != 0;
}

uint8_t TCPHeader::GetDataOffset() const {
    return (flags >> 12) & 0x0F;
}

void TCPHeader::ConvertToNetworkOrder() {
    source   = htons(source);
    dest     = htons(dest);
    seq      = htonl(seq);
    ack      = htonl(ack);
    flags    = htons(flags);     // Includes Data Offset and control bits
    window   = htons(window);
    check    = htons(check);
    urg_ptr  = htons(urg_ptr);
}

void TCPHeader::ConvertToHostOrder() {
    source   = ntohs(source);
    dest     = ntohs(dest);
    seq      = ntohl(seq);
    ack      = ntohl(ack);
    flags    = ntohs(flags);     // Reverts Data Offset and control bits
    window   = ntohs(window);
    check    = ntohs(check);
    urg_ptr  = ntohs(urg_ptr);
}