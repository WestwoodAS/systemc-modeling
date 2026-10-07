#include "bmp_utils.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

static void compare_pair(const std::string& golden_path, const std::string& target_path, const std::string& label) {
    const GrayImage golden = read_gray_bmp(golden_path);
    const GrayImage target = read_gray_bmp(target_path);

    if (golden.width != target.width || golden.height != target.height) {
        throw std::runtime_error("Image size mismatch between " + golden_path + " and " + target_path);
    }

    double sum_abs_error = 0.0;
    int max_abs_error = 0;

    for (std::size_t i = 0; i < golden.data.size(); ++i) {
        const int diff = std::abs(static_cast<int>(golden.data[i]) - static_cast<int>(target.data[i]));
        sum_abs_error += static_cast<double>(diff);
        if (diff > max_abs_error) {
            max_abs_error = diff;
        }
    }

    const double avg_abs_error = sum_abs_error / static_cast<double>(golden.data.size());

    std::cout << std::left << std::setw(28) << label
              << " Max Absolute Error = " << std::setw(5) << max_abs_error
              << " Average Absolute Error = " << avg_abs_error << '\n';
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0]
                  << " <version1_Y.bmp> <version2_Y.bmp> <version3_Y.bmp> <version4_Y.bmp>\n";
        return 1;
    }

    try {
        compare_pair(argv[1], argv[2], "Version 1 vs Version 2");
        compare_pair(argv[1], argv[3], "Version 1 vs Version 3");
        compare_pair(argv[1], argv[4], "Version 1 vs Version 4");
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
