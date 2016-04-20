#ifndef __MASTER_WRAPPER_H__
#define __MASTER_WRAPPER_H__

#include <systemc.h>
#include "simple_bus_non_blocking_if.h" // using 'simple_bus_blocking_if' inteface

class master_wrapper : public sc_module
{
public:
	// Port declaration
	sc_in_clk        clk;
	sc_in<bool>      LD;
	sc_in<bool>      RW;
	sc_in<unsigned>  X;
	sc_in<unsigned>  Y;
	sc_inout_rv<32>  D;
	sc_port<simple_bus_non_blocking_if> master_wrapper_out;

	SC_HAS_PROCESS(master_wrapper);
	master_wrapper(sc_module_name name, unsigned int size);
	void main(void);
	enum operation { WRITE = false, READ = true };
private:
	unsigned int size = 8; // default 2d array size
	sc_event m_interrupt;
};
#endif