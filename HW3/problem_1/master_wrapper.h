#ifndef __MASTER_WRAPPER_H__
#define __MASTER_WRAPPER_H__

#include <systemc.h>
#include "simple_bus_blocking_if.h" // using 'simple_bus_blocking_if' inteface
#include "mem_if.h"	// implementing the 'mem_if' interface

class master_wrapper : public sc_channel, public mem_if
{
public:
	// Port declaration
	sc_port<simple_bus_blocking_if> master_wrapper_out;

	SC_HAS_PROCESS(master_wrapper);
	master_wrapper(sc_module_name name, unsigned int size);


	// Implementing the interface 
	void word_read(unsigned x, unsigned y, int& d);
	void word_write(unsigned x, unsigned y, int d);
	void notify() { m_interrupt.notify(); }
	const sc_event& default_event() const { return m_interrupt; }

private:
	int **local_mem;
	unsigned int size = 8; // default 2d array size
	void printLocalMem(int size, char *state);
	sc_event m_interrupt;
};
#endif