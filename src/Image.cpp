#include "Image.h"
#include <opencv2/opencv.hpp>

#include "Application.h"

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

Image::Image(std::string str, int width, int height)
{
    pixels = std::vector<std::array<uint8_t, 4>>();
    this->width = width;
    this->height = height;

    for (int i = 0; i < str.size(); i += 4)
    {
        uint8_t r = str[i];
        uint8_t g = str[i + 1];
        uint8_t b = str[i + 2];
        uint8_t a = str[i + 3];

        pixels.push_back(std::array<uint8_t, 4>{r, g, b, a});
    }
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

int Image::Resize(Image *resized, int newWidth, int newHeight) const
{
    // Copy data into Mat
    cv::Mat mat(height, width, CV_8UC4);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const std::array<uint8_t, 4>& pixel = pixels[y * width + x];
            // Access the pixel at (y, x) in the Mat
            cv::Vec4b& matPixel = mat.at<cv::Vec4b>(y, x);
            matPixel[0] = pixel[2]; // Blue
            matPixel[1] = pixel[1]; // Green
            matPixel[2] = pixel[0]; // Red
            matPixel[3] = pixel[3]; // Alpha
        }
    }

    cv::Mat sized;
    if (newHeight != -1 && newWidth == -1)
    {
        int resultedWidth = (double)width / (double)height * (double)newHeight;
        cv::resize(mat, sized, cv::Size(resultedWidth, newHeight));
    }
    else if (newHeight == -1 && newWidth != -1)
    {
        int resultedHeight = (double)height / (double)width * (double)newWidth;
        cv::resize(mat, sized, cv::Size(newWidth, resultedHeight));
    }
    else if (newHeight != -1 && newWidth != -1)
        cv::resize(mat, sized, cv::Size(newWidth, newHeight));
    else
    {
        Application::Log("Error resize image: invalid parameters");
        return 1;
    }

    // Access pixel data
    std::vector pixels(std::vector(sized.datastart, sized.dataend));
    *resized = Image(pixels, sized.cols, sized.rows);
    return 0;
}

int Image::FromCompressedString(std::string str, Image* result)
{
    // Decode JPEG string into cv::Mat
    std::vector<uchar> buffer(str.begin(), str.end());
    cv::Mat bgr = cv::imdecode(buffer, cv::IMREAD_COLOR);

    if (bgr.empty())
    {
        Application::Log("Failed to decompress image");
        return 1;
    }

    // Convert BGR to RGBA
    cv::Mat rgba;
    cv::cvtColor(bgr, rgba, cv::COLOR_BGR2RGBA);

    // Copy to pixel vector
    std::vector<std::array<uint8_t, 4>> pixels(bgr.cols * bgr.rows);
    std::memcpy(pixels.data(), rgba.data, rgba.total() * rgba.elemSize());

    *result = Image(pixels, bgr.cols, bgr.rows);
    return 0;
}

std::string Image::Compress(int quality)
{
    // Convert to cv::Mat
    cv::Mat mat(height, width, CV_8UC4, pixels.data());

    // Convert from RGBA to BGR (JPEG doesn't support alpha)
    cv::Mat bgr;
    cv::cvtColor(mat, bgr, cv::COLOR_RGBA2BGR);

    // Encode to JPEG
    std::vector<uchar> buffer;
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, quality};
    cv::imencode(".jpg", bgr, buffer, params);

    // Convert to string
    return std::string(buffer.begin(), buffer.end());
}