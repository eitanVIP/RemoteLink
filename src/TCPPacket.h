#pragma once

#include <string>
#include "TCPHeader.h"

struct TCPPacket
{
    TCPHeader header;
    std::string data;
};
