#ifndef MONITOR_H_
#define MONITOR_H_

#include <systemc>
#include <iostream>

SC_MODULE(monitor) {
    sc_core::sc_port<sc_core::sc_fifo_in_if<int>> re;

    void monitor_proc();

    SC_CTOR(monitor) {
        SC_THREAD(monitor_proc);
    }
};

#endif
