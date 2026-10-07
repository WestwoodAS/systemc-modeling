#include <systemc>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace sc_core;
using namespace std;

SC_MODULE(consumer) {
    sc_in_clk iclk;
    sc_fifo_in<int> data_in;
    int data_value;

    SC_CTOR(consumer) : data_value(0) {
        SC_CTHREAD(x_consumer, iclk.pos());
    }

    void x_consumer() {
        while (true) {
            wait(3);
            data_value = data_in.read();
            cout << "At time " << sc_time_stamp()
                 << " consumes data " << data_value << endl;
        }
    }
};

SC_MODULE(producer) {
    sc_in_clk iclk;
    sc_fifo_out<int> data_out;
    int data_value;

    SC_CTOR(producer) : data_value(0) {
        SC_CTHREAD(x_producer, iclk.pos());
    }

    void x_producer() {
        while (true) {
            wait(2);
            data_value = std::rand() % 100;
            data_out.write(data_value);
            cout << "At time " << sc_time_stamp()
                 << " produces data " << data_value << endl;
        }
    }
};

int sc_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::srand(1);

    const sc_time t_PERIOD(5, SC_NS);
    sc_clock clk("clk", t_PERIOD);
    sc_fifo<int> x_fifo(8);

    producer x_producer("x_producer");
    consumer x_consumer("x_consumer");

    x_producer.iclk(clk);
    x_consumer.iclk(clk);
    x_producer.data_out(x_fifo);
    x_consumer.data_in(x_fifo);

    sc_start(200, SC_NS);
    return 0;
}
