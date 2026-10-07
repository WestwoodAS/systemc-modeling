#define SC_INCLUDE_FX
#include <systemc>

#include "bmp_utils.h"

#include <iostream>
#include <vector>

using namespace sc_core;
using namespace sc_dt;

static std::uint8_t fixed_to_u8(const sc_fixed<32, 12, SC_RND, SC_SAT>& value) {
    return static_cast<std::uint8_t>(clamp_to_u8(value.to_double()));
}

SC_MODULE(RGB2YUV_FX) {
    sc_in<sc_uint<8>> r;
    sc_in<sc_uint<8>> g;
    sc_in<sc_uint<8>> b;

    sc_out<sc_uint<8>> y;
    sc_out<sc_uint<8>> u;
    sc_out<sc_uint<8>> v;

    void compute() {
        const sc_ufixed<16, 8, SC_RND, SC_SAT> rf = static_cast<unsigned int>(r.read());
        const sc_ufixed<16, 8, SC_RND, SC_SAT> gf = static_cast<unsigned int>(g.read());
        const sc_ufixed<16, 8, SC_RND, SC_SAT> bf = static_cast<unsigned int>(b.read());

        const sc_fixed<32, 12, SC_RND, SC_SAT> yv = 0.299 * rf + 0.587 * gf + 0.114 * bf;
        const sc_fixed<32, 12, SC_RND, SC_SAT> uv = -0.169 * rf - 0.331 * gf + 0.500 * bf + 128.0;
        const sc_fixed<32, 12, SC_RND, SC_SAT> vv = 0.500 * rf - 0.419 * gf - 0.081 * bf + 128.0;

        y.write(sc_uint<8>(fixed_to_u8(yv)));
        u.write(sc_uint<8>(fixed_to_u8(uv)));
        v.write(sc_uint<8>(fixed_to_u8(vv)));
    }

    SC_CTOR(RGB2YUV_FX) {
        SC_METHOD(compute);
        sensitive << r << g << b;
    }
};

int sc_main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.bmp> <output_prefix>\n";
        return 1;
    }

    try {
        const std::string input_path = argv[1];
        const std::string output_prefix = argv[2];

        const RgbImage image = read_bmp24(input_path);
        std::vector<std::uint8_t> y_data(image.data.size());
        std::vector<std::uint8_t> u_data(image.data.size());
        std::vector<std::uint8_t> v_data(image.data.size());

        sc_signal<sc_uint<8>> r_sig("r_sig"), g_sig("g_sig"), b_sig("b_sig");
        sc_signal<sc_uint<8>> y_sig("y_sig"), u_sig("u_sig"), v_sig("v_sig");

        RGB2YUV_FX dut("dut");
        dut.r(r_sig);
        dut.g(g_sig);
        dut.b(b_sig);
        dut.y(y_sig);
        dut.u(u_sig);
        dut.v(v_sig);

        for (std::size_t i = 0; i < image.data.size(); ++i) {
            r_sig.write(image.data[i].r);
            g_sig.write(image.data[i].g);
            b_sig.write(image.data[i].b);
        
            sc_start(1, SC_NS);
        
            y_data[i] = static_cast<std::uint8_t>(y_sig.read().to_uint());
            u_data[i] = static_cast<std::uint8_t>(u_sig.read().to_uint());
            v_data[i] = static_cast<std::uint8_t>(v_sig.read().to_uint());
        }

        save_yuv_triplet_as_bmp_prefix(output_prefix, image.width, image.height, y_data, u_data, v_data);
        std::cout << "Version 4 finished: " << output_prefix << "_Y.bmp / _U.bmp / _V.bmp\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
