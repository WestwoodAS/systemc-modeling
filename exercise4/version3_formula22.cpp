#include "bmp_utils.h"

#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.bmp> <output_prefix>\n";
        return 1;
    }

    try {
        const std::string input_path = argv[1];
        const std::string output_prefix = argv[2];

        const RgbImage image = read_bmp24(input_path);
        std::vector<std::uint8_t> y(image.data.size());
        std::vector<std::uint8_t> u(image.data.size());
        std::vector<std::uint8_t> v(image.data.size());

        for (std::size_t i = 0; i < image.data.size(); ++i) {
            const int r = image.data[i].r;
            const int g = image.data[i].g;
            const int b = image.data[i].b;

            const int yy = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
            const int uu = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
            const int vv = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

            y[i] = static_cast<std::uint8_t>(clamp_to_u8_int(yy));
            u[i] = static_cast<std::uint8_t>(clamp_to_u8_int(uu));
            v[i] = static_cast<std::uint8_t>(clamp_to_u8_int(vv));
        }

        save_yuv_triplet_as_bmp_prefix(output_prefix, image.width, image.height, y, u, v);
        std::cout << "Version 3 finished: " << output_prefix << "_Y.bmp / _U.bmp / _V.bmp\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
