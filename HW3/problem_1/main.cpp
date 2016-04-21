#include <iostream>
using std::cout;
using std::endl;

#include <systemc.h>
#include "interchange.h"
#include "master_wrapper.h"
#include "simple_bus.h"
#include "simple_bus_arbiter.h"
#include "slave_wrapper.h"
#include "memory.h"

class top : public sc_module
{
public:
	// port
	sc_in_clk clk;
	// submodules
	interchange interchange_inst;
	master_wrapper ms_wrap_inst;
	simple_bus simple_bus_inst;
	simple_bus_arbiter	arbiter_inst;
	slave_wrapper sl_wrap_inst;
	memory mem_inst;
	// Declare & initialize all the submodules in a top module
	top(sc_module_name name, unsigned int size)
		: sc_module(name),
		interchange_inst("Interchange", size),
		ms_wrap_inst("Master wrapper", size),
		simple_bus_inst("Simple bus"),
		arbiter_inst("Arbiter"),
		sl_wrap_inst("Slave wrapper", size, 0, 100000),
		mem_inst("Memory", size)
	{
		interchange_inst.mem_port(ms_wrap_inst);
		ms_wrap_inst.master_wrapper_out(simple_bus_inst);
		simple_bus_inst.slave_port(sl_wrap_inst);
		simple_bus_inst.arbiter_port(arbiter_inst);
		simple_bus_inst.clock(clk);
		sl_wrap_inst.slave_wrapper_out(mem_inst);
	}
};

#define clockcycle   10     //10ns=100MHz
#define stim_len     30000   //ns

int sc_main(int argc, char* argv[]) {
	// simulation timing setup
	sc_time     stim_t(stim_len, SC_NS);
	sc_clock    clk("clk", clockcycle, 0.5, 0, SC_NS);
	top top1("Top1", 8);
	SC_HAS_PROCESS(top);
	top1.clk(clk);
	sc_start(stim_t);
	return 0;
}