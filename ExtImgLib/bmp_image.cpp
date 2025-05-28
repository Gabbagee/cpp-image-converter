#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <iostream>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

constexpr uint16_t BMP_COLOR_DEPTH = 24;

static int GetBMPStride(int width) {
    const int bytes_per_pixel = BMP_COLOR_DEPTH / 8;
    const int dword_alignment = 4;
    return dword_alignment * ((width * bytes_per_pixel + (dword_alignment - 1)) / dword_alignment);
}

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    BitmapInfoHeader(int width, int height)
        : biWidth(width), biHeight(height) {
        biDataSize = GetBMPStride(width) * height;
    }
    uint32_t biSize = sizeof(BitmapInfoHeader);
    int32_t biWidth = 0;
    int32_t biHeight = 0;
    uint16_t biPlanes = 1;
    uint16_t biColorDepth = BMP_COLOR_DEPTH;
    uint32_t biCompressionValue = 0;
    uint32_t biDataSize = 0;
    int32_t biXResolution = 11811;
    int32_t biYResolution = 11811;
    int32_t biColorPalette = 0;
    int32_t biTotalUsedColors = 0x1000000;
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapFileHeader {
    BitmapFileHeader(int width, int height) {
        bfSize = GetBMPStride(width) * height + bfDataIndent;
    }
    
    char bfType[2] = {'B', 'M'};
    uint32_t bfSize = 0;
    uint32_t bfReserved = 0;
    uint32_t bfDataIndent = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
}
PACKED_STRUCT_END

bool SaveBMP(const Path& file, const Image& image) {
    ofstream ofs(file, ios::binary);

    const int width = image.GetWidth();
    const int height = image.GetHeight();

    BitmapFileHeader file_header(width, height);
    BitmapInfoHeader info_header(width, height);

    ofs.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    ofs.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    const int stride = GetBMPStride(width);
    vector<char> buff(stride, 0);

    for (int y = height - 1; y >= 0; --y) {
        const Color* line = image.GetLine(y);
        
        for (int x = 0; x < width; ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        ofs.write(reinterpret_cast<const char*>(buff.data()), buff.size());
    }
    return ofs.good();
}

Image LoadBMP(const Path& file) {
    ifstream ifs(file, ios::binary);
    BitmapFileHeader file_header(0, 0);
    BitmapInfoHeader info_header(0, 0);

    if (!ifs.read(reinterpret_cast<char*>(&file_header), sizeof(file_header))) {
        cerr << "Error: Failed to read BMP file header from " << file << endl;
        return {};
    }

    if (file_header.bfType[0] != 'B' || file_header.bfType[1] != 'M') {
        cerr << "Error: Not a BMP file. Invalid signature in " << file << endl;
        return {};
    }

    if (!ifs.read(reinterpret_cast<char*>(&info_header), sizeof(info_header))) {
        cerr << "Error: Failed to read BMP info header from " << file << endl;
        return {};
    }

    const int width = info_header.biWidth;
    const int height = info_header.biHeight;
    const int stride = GetBMPStride(width);

    Image result(width, height, Color::Black());
    vector<char> buff(stride);

    ifs.seekg(file_header.bfDataIndent);

    for (int y = height - 1; y >= 0; --y) {
        Color* line = result.GetLine(y);
        ifs.read(buff.data(), stride);

        for (int x = 0; x < width; ++x) {
            line[x].b = static_cast<byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].r = static_cast<byte>(buff[x * 3 + 2]);
        }
    }
    return result;
}

}  // namespace img_lib