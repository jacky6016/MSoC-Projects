#include "slave_wrapper.h"

slave_wrapper::slave_wrapper(sc_module_name name, unsigned int size, unsigned int start_addr, unsigned int end_addr) : sc_channel(name), size(size), m_start_address(start_addr), m_end_address(end_addr)
{ }

simple_bus_status slave_wrapper::read(int *data, unsigned int address)
{
	// retrieve data from size * size array, where each element is of size 4 bytes
	//slave_wrapper_out->word_read(((address - m_start_address) / 4) / size, ((address - m_start_address) / 4) % size, *data);
	unsigned int x = (address - m_start_address) / 4 / size;
	unsigned int y = (address - m_start_address) / 4 % size;
	wait(clk->posedge_event());
	LD->write(true);
	RW->write(READ);
	X->write(x);
	Y->write(y);
	*data = D->read().to_long();
	// LD -> false 
	wait(clk->posedge_event());
	LD->write(false);
	return SIMPLE_BUS_OK;
}

simple_bus_status slave_wrapper::write(int *data, unsigned int address)
{
	//slave_wrapper_out->word_write(((address - m_start_address) / 4) / size, ((address - m_start_address) / 4) % size, *data);
	unsigned int x = (address - m_start_address) / 4 / size;
	unsigned int y = (address - m_start_address) / 4 % size;
	wait(clk->posedge_event());
	LD->write(true);
	RW->write(WRITE);
	X->write(x);
	Y->write(y);
	D->write(*data);
	// read in the following cycle; LD -> false 
	wait(clk->posedge_event());
	LD->write(false);
	return SIMPLE_BUS_OK;
}

inline unsigned int slave_wrapper::start_address() const
{
	return m_start_address;
}

inline unsigned int slave_wrapper::end_address() const
{
	return m_end_address;
}
