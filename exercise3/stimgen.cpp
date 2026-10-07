#include "stimgen.h"

void stimgen::stim_proc() {
    for (int i = 0; i < 20; ++i) {
        const int temp = seed + 1;
        in1->write(temp);
        in2->write(temp + 5);
        seed = (seed + 19) % 123;
    }
    wait(10, sc_core::SC_NS);
    sc_core::sc_stop();
}
