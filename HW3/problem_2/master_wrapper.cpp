#include "master_wrapper.h"

master_wrapper::master_wrapper(sc_module_name name, unsigned int size) : sc_channel(name)
{
	SC_THREAD(main);
}

void master_wrapper::main(void)
{
	int d = 0;
	for (;;)
	{
		// triggered at negedge clock
		wait(clk->negedge_event());
		// Initial and idle output value
		D->write(0);
		// Read
		if (LD->read() == true && RW->read() == true)
		{
			unsigned int x = X->read(), y = Y->read();
			unsigned int addr = 4* (x * size + y);
			master_wrapper_out->read(0, &d, addr);

			// wait until the bus have returned the value
			while (master_wrapper_out->get_status(0) != SIMPLE_BUS_OK)
			{
				wait(0.5, SC_NS);
				wait(SC_ZERO_TIME);
			}
			wait(clk->negedge_event());
			D->write(d);	
		}
		// Write
		else if (LD->read() == true && RW->read() == false)
		{
			unsigned int x = X->read(), y = Y->read();
			unsigned int addr = 4 * (x * size + y);
			if (x != y)
				d = D->read().to_long();
			master_wrapper_out->write(0, &d, addr);
			// wait until the bus has finished writing
			while (master_wrapper_out->get_status(0) != SIMPLE_BUS_OK) {
				wait(0.5, SC_NS);
				wait(SC_ZERO_TIME);
			}
			wait(clk->negedge_event());
		}
		else
		{			
			D->write(0);
		}
			
	}
}
