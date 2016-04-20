#include <systemc.h>
#include "interchange.h"


interchange::interchange(sc_module_name name, int size) : sc_module(name)
{
	local_mem = new int*[size];
	for (int i = 0; i < size; ++i) {
		local_mem[i] = new int[size];
	}
	this->size = size;
	SC_THREAD(main);
}

void interchange::main()
{
	bool done = false;
	int tmp;
	
	// re-write functional behavior to pin-cycle accurate model
	
	// copy the memory content in 2D mem-array to local memory

	// Initial port values
	wait(clk->posedge_event());
	LD->write(false);
	RW->write(READ);
	X->write(0);
	Y->write(0);
	D->write(0);

	// read into local memory
	cout << "Reading memory content" << endl;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			wait(clk->posedge_event());
			LD->write(true);
			RW->write(READ);
			X->write(i);
			Y->write(j);
			// read in the following cycle; LD -> false 
			// wait multiple cycles for response
			for (int k = 0; k < 5; k++)
				wait(clk->posedge_event());
			LD->write(false);
			local_mem[i][j] = D->read().to_long();
		}
	}

	// swap procedure
	for (int i = 0; i < size; i++)
	{
		for (int j = i+1; j < size; j++)
		{
			tmp = local_mem[i][j];
			local_mem[i][j] = local_mem[j][i];
			local_mem[j][i] = tmp;
		}		
	}
	/*
	cout << "Print interchange local memory" << endl;
	for (int i = size - 1; i >= 0; i--)
	{
		cout << "[ ";
		for (int j = 0; j < size; j++)
		{
			printf("%2d ", local_mem[i][j]);
		}
		cout << "]" << endl;
	}
	*/
	// write the updated content into 2D mem-array 
	cout << "Writing memory content" << endl;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			wait(clk->posedge_event());
			LD->write(true);
			RW->write(WRITE);
			X->write(i);
			Y->write(j);
			D->write(local_mem[i][j]);
			// LD -> false 
			// wait multiple cycles for response
			for (int k = 0; k < 5; k++)
				wait(clk->posedge_event());
			LD->write(false);			
		}
	}
}
