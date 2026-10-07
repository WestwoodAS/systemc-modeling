#include <systemc>
#include "adder.h"
#include "stimgen.h"
#include "monitor.h"

int sc_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    sc_core::sc_fifo<int> sig_a(16), sig_b(16), sig_c(16);

    Adder my_adder("my_adder");
    stimgen stin("stin");
    monitor mon("mon");

    stin.in1(sig_a);
    stin.in2(sig_b);

    my_adder.a(sig_a);
    my_adder.b(sig_b);
    my_adder.c(sig_c);

    mon.re(sig_c);

    sc_core::sc_start();
    return 0;
}
