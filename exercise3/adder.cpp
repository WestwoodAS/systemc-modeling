#include "adder.h"

void Adder::compute() {
    while (true) {
        const int a0 = a->read();
        const int b0 = b->read();
        c->write(a0 + b0);

        const int a1 = a->read();
        const int b1 = b->read();
        c->write(a1 + b1 + 2);
    }
}
