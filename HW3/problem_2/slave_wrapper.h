#ifndef __SLAVE_WRAPPER_H__
#define __SLAVE_WRAPPER_H__

#include "systemc.h"
#include "simple_bus_slave_if.h"

SC_MODULE(slave_wrapper), simple_bus_slave_if{
	//ports
	sc_in_clk         clk;
	sc_out<bool>      LD;
	sc_out<bool>      RW;
	sc_out<unsigned>  X;
	sc_out<unsigned>  Y;
	sc_inout_rv<32>   D;

	SC_HAS_PROCESS(slave_wrapper);
	slave_wrapper(sc_module_name name_, unsigned int start_addr, unsigned int end_addr, unsigned int nr_wait_states);

	void wait_loop() { if (m_wait_count >= 0) m_wait_count--; }

	bool direct_read(int *data, unsigned int addr) { return true; }
	bool direct_write(int *data, unsigned int addr) { return true; }

	// Implementing the simple_bus_slave_if interface
	simple_bus_status read(int *data, unsigned int address);
	simple_bus_status write(int *data, unsigned int address);
	unsigned int start_address() const { return m_start_addr; }
	unsigned int end_address() const { return m_end_addr; }


private:
	unsigned int m_start_addr, m_end_addr;
	unsigned int m_nr_wait_states;
	int m_wait_count;
};

#endif