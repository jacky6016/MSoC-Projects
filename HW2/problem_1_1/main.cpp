#include <iostream>
using std::cout;
using std::endl;

#include <systemc.h>
#include "Interchange.h"
#include "Memory.h"

class top : public sc_module
{
public:
	interchange interchange_inst;
	memory mem_inst;
	// Declare & initialize all the submodules in a top module
	top(sc_module_name name)
		: sc_module(name),
		interchange_inst("Interchange_inst", 8),
		mem_inst("Memory_inst1", 8)
	{
		interchange_inst.mem_port(mem_inst);
	}
};

unsigned errors = 0;
char* simulation_name = "problem_1_1";

int sc_main(int argc, char* argv[]) {
	top top1("Top1");	
	sc_start();
	return 0;
}