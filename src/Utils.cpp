#include "Utils.h"
#include "Application.h"
#include "Socket.h"

#include <cmath>
#include <unistd.h>
#include <sstream>
#include <ifaddrs.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#undef Status  // undefine the X11 macro
#include <opencv2/opencv.hpp>

namespace Utils
{
    std::string GetSocketErrorString() {
        return strerror(errno);
    }

    IPAddress GetLocalIP()
    {
        struct ifaddrs *interfaces = nullptr;
        struct ifaddrs *ifa = nullptr;
        void *tmpAddrPtr = nullptr;
        string localIP = "";

        // Get the list of interfaces
        if (getifaddrs(&interfaces) == 0)
        {
            // Loop through the list of interfaces
            for (ifa = interfaces; ifa != nullptr; ifa = ifa->ifa_next)
            {
                if (ifa->ifa_addr->sa_family == AF_INET) // Check if it's an IPv4 address
                {
                    // Get the IP address of the interface
                    tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    char ipStr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, tmpAddrPtr, ipStr, INET_ADDRSTRLEN);

                    // Exclude the loopback address (127.0.0.1)
                    if (strcmp(ifa->ifa_name, "lo") != 0)
                    {
                        localIP = string(ipStr);
                        break; // Exit after finding the first non-loopback IP
                    }
                }
            }
        }
        else
        {
            Application::Log("Error getting network interfaces: " + string(strerror(errno)));
        }

        // Free the memory allocated by getifaddrs
        if (interfaces != nullptr)
        {
            freeifaddrs(interfaces);
        }

        if (localIP.empty())
        {
            Application::Log("No valid IP address found!");
        }
        else
        {
            Application::Log("Local IP address: " + localIP);
        }

        // Return the IPAddress object with the found IP address
        return IPAddress(localIP, NetworkNumber<Port>(0, NumberType::Host));  // Assuming IPAddress class constructor accepts a string
    }

    std::string PacketToString(IPHeader ipHeader, TCPHeader tcpHeader, string data)
    {
        std::stringstream ss;
    
        // Convert IP addresses from uint32_t to readable format
        uint8_t* src = reinterpret_cast<uint8_t*>(&ipHeader.src_ip);
        uint8_t* dest = reinterpret_cast<uint8_t*>(&ipHeader.dest_ip);
    
        ss << "Packet(";
    
        ss << "IP: "
           << int(src[0]) << "." << int(src[1]) << "." << int(src[2]) << "." << int(src[3])
           << " -> "
           << int(dest[0]) << "." << int(dest[1]) << "." << int(dest[2]) << "." << int(dest[3])
           << ", ";
    
        ss << "TCP: " 
           << tcpHeader.source << " -> " << tcpHeader.dest
           << ", ";
    
        ss << "SEQ: " << tcpHeader.seq
           << ", ACK: " << tcpHeader.ack
           << ", Flags: ";
    
        if (tcpHeader.GetFlagFIN()) ss << "FIN ";
        if (tcpHeader.GetFlagSYN()) ss << "SYN ";
        if (tcpHeader.GetFlagRST()) ss << "RST ";
        if (tcpHeader.GetFlagPSH()) ss << "PSH ";
        if (tcpHeader.GetFlagACK()) ss << "ACK ";
        if (tcpHeader.GetFlagURG()) ss << "URG ";

        ss << ", Data: " << data;
    
        ss << ")";
    
        return ss.str();
    }

    uint16_t CalculateIPChecksum(uint16_t* data, size_t length) {
        uint32_t sum = 0;

        while (length > 1) {
            sum += *data++;
            length -= 2;
        }

        if (length == 1) { // Handle odd byte at the end
            sum += *(uint8_t*)data;
        }

        sum = (sum >> 16) + (sum & 0xFFFF); // Fold high into low
        sum += (sum >> 16);                 // Handle carry
        return (uint16_t)~sum;
    }

    IPHeader CreateIPHeader(IPAddress srcIP, IPAddress destIP, size_t dataLength)
    {
        IPHeader header;

        // Version 4, IHL 5 (20 bytes)
        header.version_ihl = (4 << 4) | 5;
        // Default TOS
        header.tos = 0;
        // IPHeader + TCPHeader + data
        header.total_length = htons(sizeof(IPHeader) + sizeof(TCPHeader) + dataLength);
        // Arbitrary ID
        header.identification = htons(round(10000 * rand() / (RAND_MAX + 1.0) + 5000));
        // Don't fragment
        header.flags_offset = htons(0x4000);
        // Common TTL value
        header.ttl = 64;
        // TCP protocol
        header.protocol = 6;
        // Network byte order
        header.src_ip = srcIP.GetAsNetworkBinary();
        // Network byte order
        header.dest_ip = destIP.GetAsNetworkBinary();
        // Checksum calculation
        header.checksum = 0;
        header.checksum = CalculateIPChecksum((uint16_t*)&header, sizeof(IPHeader));

        return header;
    }

    struct PseudoHeader
    {
        uint32_t srcIP;
        uint32_t destIP;
        uint8_t reserved;  // Must be 0
        uint8_t protocol;
        uint16_t tcpLength;  // TCP header + payload
    };

    uint16_t CalculateTCPChecksum(TCPHeader* headerPtr, size_t tcpLength, IPAddress srcIP, IPAddress destIP) {
        headerPtr->check = 0;

        struct PseudoHeader pheader;
        pheader.srcIP = srcIP.GetAsNetworkBinary();
        pheader.destIP = destIP.GetAsNetworkBinary();
        pheader.reserved = 0;
        pheader.protocol = 6; // TCP protocol number
        pheader.tcpLength = htons(tcpLength);

        uint32_t sum = 0;
        uint16_t* pheader_ptr = (uint16_t*)&pheader;

        // Add pseudo-header
        for (size_t i = 0; i < sizeof(PseudoHeader) / 2; ++i) {
            sum += *pheader_ptr++;
        }

        uint16_t* segmentPtr = (uint16_t*)headerPtr;
        // Add TCP segment (header + data)
        while (tcpLength > 1) {
            sum += *segmentPtr++;
            tcpLength -= 2;
        }

        if (tcpLength == 1) { // Handle odd byte
            sum += *(uint8_t*)segmentPtr;
        }

        sum = (sum >> 16) + (sum & 0xFFFF); // Fold high into low
        sum += (sum >> 16);                 // Handle carry
        return (uint16_t)~sum;
    }

    void CreateInitialTCPHeader(TCPHeader& header, NetworkNumber<Port> sourcePort, NetworkNumber<Port> destPort)
    {
        //Set the first message tcp header
        header.source = sourcePort.GetAsHost();
        header.dest = destPort.GetAsHost();
        header.seq = 0;
        header.ack = 0;
        header.flags = 0;
        //SYN is set because first message
        header.SetDataOffset(5);
        header.SetFlagSYN(true);
        //Max 16-bit number for maximum messages size
        header.window = 0xFFFF;
        header.check = 0;
        header.urg_ptr = 0;
    }

    int GetRandomPort() {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            Application::Log("Failed to create socket on generating random port");
            return -1;
        }

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces
        addr.sin_port = 0; // Let OS choose a free port

        if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
            Application::Log("Failed to bind socket on generating random port");
            close(sock);
            return -1;
        }

        socklen_t len = sizeof(addr);
        if (getsockname(sock, (sockaddr*)&addr, &len) == -1) {
            Application::Log("Failed to get port from socket on generating random port");
            close(sock);
            return -1;
        }

        int port = ntohs(addr.sin_port);
        close(sock);
        return port;
    }

    int TakeScreenshot(std::vector<uint8_t> &pixels, int targetHeight)
    {
        Display* display = XOpenDisplay(nullptr);
        if (!display) {
            Application::Log("Failed to take a screenshot: can't open display");
            return 1;
        }

        Window root = DefaultRootWindow(display);
        XWindowAttributes gwa;
        XGetWindowAttributes(display, root, &gwa);

        int width = gwa.width;
        int height = gwa.height;

        XImage* image = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);

        if (!image) {
            Application::Log("Failed to take a screenshot: can't get image");
            return 1;
        }

        // Convert XImage to OpenCV Mat (BGRA 32-bit)
        cv::Mat mat(height, width, CV_8UC4);
        for (int y = 0; y < height; ++y) {
            uint32_t* src_row = (uint32_t*)(image->data + y * image->bytes_per_line);
            for (int x = 0; x < width; ++x) {
                uint32_t pixel = src_row[x];
                cv::Vec4b& bgra = mat.at<cv::Vec4b>(y, x);
                bgra[0] = (pixel & image->blue_mask);            // Blue
                bgra[1] = (pixel & image->green_mask) >> 8;      // Green
                bgra[2] = (pixel & image->red_mask) >> 16;       // Red
                bgra[3] = 255;                                   // Alpha
            }
        }

        // Resize it
        cv::Mat resized;
        cv::resize(mat, resized, cv::Size(width / height * targetHeight, targetHeight));

        // Access pixel data
        pixels = std::vector(resized.datastart, resized.dataend);

        XDestroyImage(image);
        XCloseDisplay(display);
        return 0;
    }

    std::string IntToHexString(int value) {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << value;
        return ss.str();
    }
    int HexStringToInt(const std::string& hexStr) {
        return std::stoi(hexStr, nullptr, 16);
    }
}
