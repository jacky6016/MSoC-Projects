#include <iostream>
using std::cout;
using std::endl;

#include <systemc.h>
#include "interchange.h"
#include "memory.h"

class top : public sc_module
{
public:
	interchange interchange_inst;
	memory mem_inst;

	sc_in_clk clk;
	sc_signal<bool>      LD;
	sc_signal<bool>      RW;
	sc_signal<unsigned>  X;
	sc_signal<unsigned>  Y;
	sc_signal_rv<32>     D;

	// Constructor: initialize all the submodules in the top module
	top(sc_module_name name)
		: sc_module(name),
		interchange_inst("Interchange_inst", 8),
		mem_inst("memory_inst", 8)
	{
		interchange_inst.clk(clk);
		interchange_inst.LD(LD);
		interchange_inst.RW(RW);
		interchange_inst.X(X);
		interchange_inst.Y(Y);
		interchange_inst.D(D);

		mem_inst.clk(clk);
		mem_inst.LD(LD);
		mem_inst.RW(RW);
		mem_inst.X(X);
		mem_inst.Y(Y);
		mem_inst.D(D);
	}
};

#define clockcycle   10     //10ns=100MHz
#define stim_len     3000   //ns

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