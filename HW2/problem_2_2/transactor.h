#ifndef __TRANSACTOR_H__
#define __TRANSACTOR_H__

#include <systemc.h>
#include "mem_if.h"

// Interface function implementation moved from "memory" to here
class transactor : public sc_module
{
public:
	// Port declaration
	sc_in_clk clk;
	sc_in<bool> LD; //true: activate operation ; false: idle
	sc_in<bool> RW; // read / write
	sc_in<unsigned> X, Y; // direction
	sc_inout_rv<32> D; // bi-direction data-bus
	sc_port<mem_if> mem_port; /* This is the interface connected to 2d mem-array */

	// Constructor declaration
	SC_HAS_PROCESS(transactor);
	transactor(sc_module_name name);

	// Processes
	void main(void);
	
};

#endif