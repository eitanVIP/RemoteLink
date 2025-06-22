#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>

class Image {
private:
    std::vector<std::array<uint8_t, 4>> pixels;
    int width;
    int height;
public:
    Image();
    Image(int width, int height);
    Image(std::vector<uint8_t> bgraVals, int width, int height);
    Image(std::vector<std::array<uint8_t, 4>> pixels, int width, int height);
    Image(std::string str, int width, int height);
    std::string GetAsString() const;
    size_t Size();
    std::vector<std::array<uint8_t, 4>> GetPixels();
    std::vector<uint8_t> GetValues();
    int GetWidth() const;
    int GetHeight() const;
    Image Resize(int newHeight);
};
