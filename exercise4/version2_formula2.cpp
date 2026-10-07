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
            const double r = static_cast<double>(image.data[i].r);
            const double g = static_cast<double>(image.data[i].g);
            const double b = static_cast<double>(image.data[i].b);

            y[i] = clamp_to_u8(0.2568 * r + 0.5041 * g + 0.0979 * b + 16.0);
            u[i] = clamp_to_u8(-0.1482 * r - 0.2910 * g + 0.4392 * b + 128.0);
            v[i] = clamp_to_u8(0.4392 * r - 0.3678 * g - 0.0714 * b + 128.0);
        }

        save_yuv_triplet_as_bmp_prefix(output_prefix, image.width, image.height, y, u, v);
        std::cout << "Version 2 finished: " << output_prefix << "_Y.bmp / _U.bmp / _V.bmp\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
