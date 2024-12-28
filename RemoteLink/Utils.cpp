#include "Utils.h"

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace Utils
{
    std::string GetWSAErrorString() {
        char* messageBuffer = nullptr;

        // Use FormatMessage to get the error string
        DWORD size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            reinterpret_cast<LPSTR>(&messageBuffer),
            0,
            nullptr);

        std::string errorMessage;

        if (size > 0 && messageBuffer != nullptr) {
            errorMessage = messageBuffer;
            // Free the buffer allocated by FormatMessage
            LocalFree(messageBuffer);
        } else {
            errorMessage = "Unknown error code: " + std::to_string(WSAGetLastError());
        }

        return errorMessage;
    }
}
