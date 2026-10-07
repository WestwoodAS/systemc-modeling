#ifndef STIMGEN_H_
#define STIMGEN_H_

#include <systemc>

SC_MODULE(stimgen) {
    sc_core::sc_port<sc_core::sc_fifo_out_if<int>> in1;
    sc_core::sc_port<sc_core::sc_fifo_out_if<int>> in2;

    int seed;

    void stim_proc();

    SC_CTOR(stimgen) : seed(10) {
        SC_THREAD(stim_proc);
    }
};

#endif
