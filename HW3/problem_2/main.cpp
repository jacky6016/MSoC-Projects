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
	// signals
	sc_signal<bool>      LD1, LD2;
	sc_signal<bool>      RW1, RW2;
	sc_signal<unsigned>  X1, X2;
	sc_signal<unsigned>  Y1, Y2;
	sc_signal_rv<32>     D1, D2;

	// Constructor: initialize all the submodules in the top module
	top(sc_module_name name)
		: sc_module(name),
		interchange_inst("Interchange_inst", 8),
		ms_wrap_inst("Master_wrapper", 8),
		simple_bus_inst("Simple_bus"),
		arbiter_inst("Arbiter"),
		sl_wrap_inst("Slave_wrapper",0, 256, 2),
		mem_inst("memory_inst", 8)
	{
		interchange_inst.clk(clk);
		interchange_inst.LD(LD1);
		interchange_inst.RW(RW1);
		interchange_inst.X(X1);
		interchange_inst.Y(Y1);
		interchange_inst.D(D1);

		ms_wrap_inst.clk(clk);
		ms_wrap_inst.LD(LD1);
		ms_wrap_inst.RW(RW1);
		ms_wrap_inst.X(X1);
		ms_wrap_inst.Y(Y1);
		ms_wrap_inst.D(D1);
		ms_wrap_inst.master_wrapper_out(simple_bus_inst);

		simple_bus_inst.clock(clk);
		simple_bus_inst.arbiter_port(arbiter_inst);
		simple_bus_inst.slave_port(sl_wrap_inst);

		sl_wrap_inst.clk(clk);
		sl_wrap_inst.LD(LD2);
		sl_wrap_inst.RW(RW2);
		sl_wrap_inst.X(X2);
		sl_wrap_inst.Y(Y2);
		sl_wrap_inst.D(D2);

		mem_inst.clk(clk);
		mem_inst.LD(LD2);
		mem_inst.RW(RW2);
		mem_inst.X(X2);
		mem_inst.Y(Y2);
		mem_inst.D(D2);
	}
};

#define clockcycle   10     //10ns=100MHz
#define stim_len     30000   //ns

int sc_main(int argc, char* argv[]) {
	// avoid warnings
	sc_report_handler::set_actions(SC_ID_VECTOR_CONTAINS_LOGIC_VALUE_, SC_DO_NOTHING);
	// simulation timing setup
	sc_time     stim_t(stim_len, SC_NS);
	sc_clock    clk("clk", clockcycle, 0.5, 0, SC_NS);
	// create a top system module
	top top_inst("Top");
	top_inst.clk(clk);
	sc_start(stim_t);
	return 0;
}