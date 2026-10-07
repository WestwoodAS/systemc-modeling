#ifndef BMP_UTILS_H_
#define BMP_UTILS_H_

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#pragma pack(push, 1)
struct BmpFileHeader {
    std::uint16_t bfType;
    std::uint32_t bfSize;
    std::uint16_t bfReserved1;
    std::uint16_t bfReserved2;
    std::uint32_t bfOffBits;
};

struct BmpInfoHeader {
    std::uint32_t biSize;
    std::int32_t biWidth;
    std::int32_t biHeight;
    std::uint16_t biPlanes;
    std::uint16_t biBitCount;
    std::uint32_t biCompression;
    std::uint32_t biSizeImage;
    std::int32_t biXPelsPerMeter;
    std::int32_t biYPelsPerMeter;
    std::uint32_t biClrUsed;
    std::uint32_t biClrImportant;
};
#pragma pack(pop)

struct Pixel {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

struct GrayImage {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> data;
};

struct RgbImage {
    int width = 0;
    int height = 0;
    std::vector<Pixel> data;
};

inline int clamp_to_u8_int(int value) {
    return std::max(0, std::min(255, value));
}

inline std::uint8_t clamp_to_u8(double value) {
    int rounded = static_cast<int>(value + (value >= 0.0 ? 0.5 : -0.5));
    rounded = clamp_to_u8_int(rounded);
    return static_cast<std::uint8_t>(rounded);
}

inline RgbImage read_bmp24(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open input BMP: " + path);
    }

    BmpFileHeader file_header{};
    BmpInfoHeader info_header{};
    in.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
    in.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));

    if (!in || file_header.bfType != 0x4D42) {
        throw std::runtime_error("Input is not a valid BMP file: " + path);
    }
    if (info_header.biBitCount != 24 || info_header.biCompression != 0) {
        throw std::runtime_error("Only uncompressed 24-bit BMP is supported: " + path);
    }
    if (info_header.biWidth <= 0 || info_header.biHeight == 0) {
        throw std::runtime_error("Unsupported BMP dimensions: " + path);
    }

    const bool bottom_up = info_header.biHeight > 0;
    const int width = info_header.biWidth;
    const int height = bottom_up ? info_header.biHeight : -info_header.biHeight;
    const int row_bytes = width * 3;
    const int padded_row_bytes = (row_bytes + 3) & ~3;

    in.seekg(file_header.bfOffBits, std::ios::beg);

    RgbImage image;
    image.width = width;
    image.height = height;
    image.data.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));

    std::vector<std::uint8_t> row(static_cast<std::size_t>(padded_row_bytes));
    for (int y = 0; y < height; ++y) {
        in.read(reinterpret_cast<char*>(row.data()), padded_row_bytes);
        if (!in) {
            throw std::runtime_error("Failed while reading BMP pixel data: " + path);
        }

        const int dst_y = bottom_up ? (height - 1 - y) : y;
        for (int x = 0; x < width; ++x) {
            const std::size_t src = static_cast<std::size_t>(x) * 3U;
            Pixel& p = image.data[static_cast<std::size_t>(dst_y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)];
            p.b = row[src + 0];
            p.g = row[src + 1];
            p.r = row[src + 2];
        }
    }

    return image;
}

inline GrayImage read_gray_bmp(const std::string& path) {
    const RgbImage rgb = read_bmp24(path);
    GrayImage gray;
    gray.width = rgb.width;
    gray.height = rgb.height;
    gray.data.resize(rgb.data.size());
    for (std::size_t i = 0; i < rgb.data.size(); ++i) {
        gray.data[i] = rgb.data[i].r;
    }
    return gray;
}

inline void write_gray_bmp(const std::string& path, int width, int height, const std::vector<std::uint8_t>& gray) {
    if (static_cast<int>(gray.size()) != width * height) {
        throw std::runtime_error("Gray buffer size does not match width * height");
    }

    const int row_bytes = width * 3;
    const int padded_row_bytes = (row_bytes + 3) & ~3;
    const std::uint32_t image_size = static_cast<std::uint32_t>(padded_row_bytes * height);

    BmpFileHeader file_header{};
    file_header.bfType = 0x4D42;
    file_header.bfOffBits = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);
    file_header.bfSize = file_header.bfOffBits + image_size;

    BmpInfoHeader info_header{};
    info_header.biSize = sizeof(BmpInfoHeader);
    info_header.biWidth = width;
    info_header.biHeight = height;
    info_header.biPlanes = 1;
    info_header.biBitCount = 24;
    info_header.biCompression = 0;
    info_header.biSizeImage = image_size;

    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Cannot create output BMP: " + path);
    }

    out.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    out.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    std::vector<std::uint8_t> row(static_cast<std::size_t>(padded_row_bytes), 0);
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            const std::uint8_t value = gray[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)];
            const std::size_t dst = static_cast<std::size_t>(x) * 3U;
            row[dst + 0] = value;
            row[dst + 1] = value;
            row[dst + 2] = value;
        }
        out.write(reinterpret_cast<const char*>(row.data()), padded_row_bytes);
    }
}

inline void save_yuv_triplet_as_bmp_prefix(const std::string& prefix,
                                           int width,
                                           int height,
                                           const std::vector<std::uint8_t>& y,
                                           const std::vector<std::uint8_t>& u,
                                           const std::vector<std::uint8_t>& v) {
    write_gray_bmp(prefix + "_Y.bmp", width, height, y);
    write_gray_bmp(prefix + "_U.bmp", width, height, u);
    write_gray_bmp(prefix + "_V.bmp", width, height, v);
}

#endif
