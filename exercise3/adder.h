#ifndef ADDER_H_
#define ADDER_H_

#include <systemc>

SC_MODULE(Adder) {
    sc_core::sc_port<sc_core::sc_fifo_in_if<int>> a;
    sc_core::sc_port<sc_core::sc_fifo_in_if<int>> b;
    sc_core::sc_port<sc_core::sc_fifo_out_if<int>> c;

    void compute();

    SC_CTOR(Adder) {
        SC_THREAD(compute);
    }
};

#endif
