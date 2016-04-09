#include "simple_process_ex.h"
void simple_process_ex::my_thread_process(void) {
    std::cout << "my_thread_process executed within "
        << name() 
        << std::endl;

    wait(10, SC_NS);
    std::cout << "Now at " << sc_time_stamp() << std::endl;
    sc_time t_DELAY (2, SC_MS); //keyboard debounce time
    t_DELAY *= 2;
    std::cout << "Delaying " << t_DELAY << std::endl;
    wait(t_DELAY);
    std::cout << "Now at " << sc_time_stamp() << std::endl;


}
