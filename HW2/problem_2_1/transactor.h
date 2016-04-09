#ifndef __TRANSACTOR_H__
#define __TRANSACTOR_H__

#include <systemc.h>
#include "mem_if.h"

// Interface function implementation moved from "memory" to here
class transactor : public sc_channel, public mem_if
{
public:
	// Port declaration
	sc_in_clk clk;
	sc_out<bool> LD; //true: activate operation ; false: idle
	sc_out<bool> RW; // read / write
	sc_out<unsigned> X, Y; // direction
	sc_inout_rv<32> D; // bi-direction data-bus
	
	// Constructor declaration
	SC_HAS_PROCESS(transactor);
	transactor(sc_module_name name);

	// Processes
	// Interface implementation
	void word_read(unsigned x, unsigned y, int& d);
	void word_write(unsigned x, unsigned y, int d);
};

#endif