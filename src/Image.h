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
    static int FromCompressedString(std::string str, Image* result);
    size_t Size();
    std::vector<std::array<uint8_t, 4>> GetPixels();
    std::vector<uint8_t> GetValues();
    int GetWidth() const;
    int GetHeight() const;
    int Resize(Image *resized, int newWidth = -1, int newHeight = -1) const;
    std::string Compress(int quality = 80);
};
