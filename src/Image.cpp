#include "Image.h"

Image::Image()
{

}

Image::Image(int width, int height)
{
    pixels = std::vector<std::array<uint8_t, 4>>();
    this->width = width;
    this->height = height;
}

Image::Image(std::vector<uint8_t> bgraVals, int width, int height)
{
    pixels = std::vector<std::array<uint8_t, 4>>();
    this->width = width;
    this->height = height;
    for (int i = 0; i < bgraVals.size(); i += 4)
        pixels.push_back(std::array<uint8_t, 4>{bgraVals[i + 2], bgraVals[i + 1], bgraVals[i], bgraVals[i + 3]});
}

Image::Image(std::vector<std::array<uint8_t, 4>> pixels, int width, int height)
{
    this->width = width;
    this->height = height;
    this->pixels = pixels;
}

std::string Image::GetAsString() const
{
    std::string str = "";
    for (int i = 0; i < pixels.size(); i++)
    {
        str += pixels[i][0];
        str += pixels[i][1];
        str += pixels[i][2];
        str += pixels[i][3];
    }

    return str;
}


Image Image::FromString(std::string str, int width, int height)
{
    Image result(width, height);
    for (int i = 0; i < str.size(); i += 4)
    {
        uint8_t r = str[i];
        uint8_t g = str[i + 1];
        uint8_t b = str[i + 2];
        uint8_t a = str[i + 3];

        result.pixels.push_back(std::array<uint8_t, 4>{r, g, b, a});
    }

    return result;
}

size_t Image::Size()
{
    return pixels.size();
}

std::vector<std::array<uint8_t, 4>> Image::GetPixels()
{
    return pixels;
}


std::vector<uint8_t> Image::GetValues()
{
    std::vector<uint8_t> vals(Size() * 4);
    for (int i = 0; i < Size(); ++i)
    {
        vals[4 * i] = pixels[i][0];
        vals[4 * i + 1] = pixels[i][1];
        vals[4 * i + 2] = pixels[i][2];
        vals[4 * i + 3] = pixels[i][3];
    }

    return vals;
}

int Image::GetWidth() const
{
    return width;
}

int Image::GetHeight() const
{
    return height;
}