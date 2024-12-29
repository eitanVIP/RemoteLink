#include "TCPHeader.h"

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

bool TCPHeader::GetFlagFIN() { 
    return (flags & (1 << 0)) != 0;
}

bool TCPHeader::GetFlagSYN() { 
    return (flags & (1 << 1)) != 0;
}

bool TCPHeader::GetFlagRST() { 
    return (flags & (1 << 2)) != 0;
}

bool TCPHeader::GetFlagPSH() { 
    return (flags & (1 << 3)) != 0;
}

bool TCPHeader::GetFlagACK() { 
    return (flags & (1 << 4)) != 0;
}

bool TCPHeader::GetFlagURG() { 
    return (flags & (1 << 5)) != 0;
}