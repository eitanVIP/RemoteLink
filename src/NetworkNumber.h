#pragma once

#include <cstdint>
#include <type_traits>
#include <arpa/inet.h>

enum class NumberType { Host, Network };

template <typename T>
class NetworkNumber {
private:
    T hNumber;
    T nNumber;

    static T HostToNetwork(T value) {
        if constexpr (std::is_same_v<T, uint16_t>) {
            return htons(value);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            return htonl(value);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return ByteSwap64(value);
        } else {
            static_assert(sizeof(T) == 0, "Unsupported type");
        }
    }

    static T NetworkToHost(T value) {
        if constexpr (std::is_same_v<T, uint16_t>) {
            return ntohs(value);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            return ntohl(value);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return ByteSwap64(value);
        } else {
            static_assert(sizeof(T) == 0, "Unsupported type");
        }
    }

    static uint64_t ByteSwap64(uint64_t value) {
        return ((value & 0x00000000000000FFULL) << 56) |
               ((value & 0x000000000000FF00ULL) << 40) |
               ((value & 0x0000000000FF0000ULL) << 24) |
               ((value & 0x00000000FF000000ULL) << 8) |
               ((value & 0x000000FF00000000ULL) >> 8) |
               ((value & 0x0000FF0000000000ULL) >> 24) |
               ((value & 0x00FF000000000000ULL) >> 40) |
               ((value & 0xFF00000000000000ULL) >> 56);
    }

public:
    NetworkNumber(T value, NumberType type) {
        if (type == NumberType::Host) {
            hNumber = value;
            nNumber = HostToNetwork(value);
        } else {
            nNumber = value;
            hNumber = NetworkToHost(value);
        }
    }

    T GetAsHost() const { return hNumber; }
    T GetAsNetwork() const { return nNumber; }
};
