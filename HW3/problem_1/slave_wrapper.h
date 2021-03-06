#ifndef __SLAVE_WRAPPER_H__
#define __SLAVE_WRAPPER_H__

#include <systemc.h>
#include "simple_bus_slave_if.h" // implementing the 'simple_bus_slave_if' interface
#include "mem_if.h"	// using 'mem_if' inteface

class slave_wrapper : public sc_channel, public simple_bus_slave_if
{
public:
	// Port declaration
	sc_port<mem_if> slave_wrapper_out;

	SC_HAS_PROCESS(slave_wrapper);
	slave_wrapper(sc_module_name name, unsigned int size, unsigned int start_addr, unsigned int end_addr);

	bool direct_read(int *data, unsigned int addr)
	{
		return (read(data, addr) == SIMPLE_BUS_OK);
	}
	bool direct_write(int *data, unsigned int addr)
	{
		return (write(data, addr) == SIMPLE_BUS_OK);
	}

	// Implementing the interface 
	simple_bus_status read(int *data, unsigned int address);
	simple_bus_status write(int *data, unsigned int address);
	unsigned int start_address() const;
	unsigned int end_address() const;

private:
	int idx;
	unsigned int m_start_address;
	unsigned int m_end_address;
	unsigned int size;
};
#endif