#include "monitor.h"

void monitor::monitor_proc() {
    while (true) {
        std::cout << "The result of the computation is = " << re->read() << std::endl;
    }
}
