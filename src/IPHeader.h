#pragma once

struct IPHeader {
    uint8_t version_ihl;       // Version (4 bits) + IHL (4 bits)
    uint8_t tos;               // Type of Service
    uint16_t total_length;     // Total length (header + data)
    uint16_t identification;   // Identification
    uint16_t flags_offset;     // Flags + Fragment Offset
    uint8_t ttl;               // Time to Live
    uint8_t protocol;          // Protocol (e.g., 6 for TCP)
    uint16_t checksum;         // Header checksum
    uint32_t src_ip;           // Source IP address
    uint32_t dest_ip;          // Destination IP address
};
