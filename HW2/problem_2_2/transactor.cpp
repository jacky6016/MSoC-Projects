#include <systemc.h>
#include "transactor.h"


transactor::transactor(sc_module_name name) : sc_module(name)
{
	SC_THREAD(main);
}

void transactor::main()
{
	// This is a buffer for the Interchange module to drive the 2d-array channel
	int tmp;
	// Initial output port value
	wait(clk->negedge_event());
	D->write(0);

	for (;;)
	{
		wait(clk->negedge_event());
		
		// Read to Interchange
		if (LD->read() == true && RW->read() == true)
		{
			mem_port->word_read(X->read(), Y->read(), tmp);
			D->write(tmp);
		}
		// Write to 2d memory
		else if (LD->read() == true && RW->read() == false)
		{
			mem_port->word_write(X->read(), Y->read(), D->read().to_long());
		}
		// Idle
		else
		{
			D->write(0);
		}
	}
}