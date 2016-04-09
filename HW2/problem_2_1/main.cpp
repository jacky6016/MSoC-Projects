#include <iostream>
using std::cout;
using std::endl;

#include <systemc.h>
#include "interchange.h"
#include "memory.h"
#include "transactor.h"

class top : public sc_module
{
public:
	interchange interchange_inst;
	memory mem_inst;
	transactor transactor_inst;

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
		transactor_inst("Transactor_inst"),
		mem_inst("memory_inst", 8)
	{
		interchange_inst.mem_port(transactor_inst);
		
		transactor_inst.clk(clk);
		transactor_inst.LD(LD);
		transactor_inst.RW(RW);
		transactor_inst.X(X);
		transactor_inst.Y(Y);
		transactor_inst.D(D);

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