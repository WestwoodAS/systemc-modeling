#include <systemc>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace sc_core;
using namespace std;

SC_MODULE(shift_reg) {
    sc_in_clk iclk;

    int q1;
    int q2;
    int q3;
    int q4;

    SC_CTOR(shift_reg) : q1(0), q2(0), q3(0), q4(0) {
        SC_CTHREAD(sync_shift_reg, iclk.pos());
    }

    void sync_shift_reg() {
        while (true) {
            wait();
            cout << "At time " << sc_time_stamp()
                 << " Q1: " << q1
                 << " Q2: " << q2
                 << " Q3: " << q3
                 << " Q4: " << q4 << endl;

            const int data_in = std::rand() % 100;
            q4 = q3;
            q3 = q2;
            q2 = q1;
            q1 = data_in;
        }
    }
};

int sc_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::srand(1);

    const sc_time t_PERIOD(20, SC_NS);
    sc_clock clk("clk", t_PERIOD);

    shift_reg ishift_reg("ishift_reg");
    ishift_reg.iclk(clk);

    sc_start(300, SC_NS);
    return 0;
}
