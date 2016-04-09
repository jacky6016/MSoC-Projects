#include <systemc.h>
#include "interchange.h"

interchange::interchange(sc_module_name name, int size) : sc_module(name)
{
	SC_THREAD(main);
}

void interchange::main()
{
	bool done = false;
	int **local_mem;
	int tmp;
	
	// infinite loop for SC_THREAD()
	
	local_mem = new int*[size];
	for (int i = 0; i < size; ++i) {
		local_mem[i] = new int[size];
	}
	this->size = size;
	
	// copy the memory content in 2D mem-array to local memory
	cout << "Reading memory content" << endl;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			mem_port->word_read(i, j, local_mem[i][j]);
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
	// write the updated content into 2D mem-array 
	cout << "Writing memory content" << endl;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			mem_port->word_write(i, j, local_mem[i][j]);
		}
	}
}
