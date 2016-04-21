#include "master_wrapper.h"

master_wrapper::master_wrapper(sc_module_name name, unsigned int size) : sc_channel(name)
{ }


void master_wrapper::word_read(unsigned x, unsigned y, int& d)
{
	// Only need to read a word
	/*
		burst_read(unsigned int unique_priority
			, int *data
			, unsigned int start_address
			, unsigned int length = 1
			, bool lock = false) = 0;
	*/
	unsigned int addr = 4 * (x * size + y);
	master_wrapper_out->burst_read(0, &d, addr);
	
}

void master_wrapper::word_write(unsigned x, unsigned y, int d)
{
	// Only need to write a word
	/*
		virtual simple_bus_status burst_write(unsigned int unique_priority
			, int *data
			, unsigned int start_address
			, unsigned int length = 1
			, bool lock = false) = 0;
	*/
	unsigned int addr = 4 * (x * size + y);
	master_wrapper_out->burst_write(0, &d, addr);

}