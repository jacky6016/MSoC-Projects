#include <time.h>
#include <iomanip>
#include <systemc.h>
#include "memory.h"

memory::memory(sc_module_name name, int size) : sc_module(name)
{
	// dynamically allocate an 2d array
	local_mem = new int*[size];
	for (int i = 0; i < size; ++i) {
		local_mem[i] = new int[size];
	}
	this->size = size;
	
	// initialize the array(randomly)
	srand(time(NULL));
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)	
			local_mem[i][j] = rand() % 100;

	// print local memory to console
	printLocalMem(size, "Original memory content");

	SC_THREAD(main);
}

memory::~memory()
{
	// print local memory to console
	printLocalMem(size, "Mirrored memory content");
	for (int i = 0; i < size; ++i) {
		delete[] local_mem[i];
	}
	delete[] local_mem;
}

void memory::main()
{
	// Use infinite for loop to emulate hardware behavior
	// and describe I/O ports behavior inside the loop
	for (;;)
	{
		// triggered at negedge clock
		wait(clk->negedge_event());
		
		
		
		// Read
		if (LD->read() == true && RW->read() == true) 
		{
			D->write(local_mem[X->read()][Y->read()]);
		}
		// Write
		else if (LD->read() == true && RW->read() == false)
		{
			if (X->read() != Y->read())
				local_mem[X->read()][Y->read()] = D->read().to_long();
		}
		else
		{
			// Initial and idle output value
			D->write(0);
		}		
	}
}

void memory::printLocalMem(int size, char *state)
{
	cout << state << endl;
	for (int i = size-1; i >= 0; i--)
	{
		cout << "[ ";
		for (int j = 0; j < size; j++)
		{
			printf("%2d ", local_mem[i][j]);
		}
		cout << "]" << endl;
	}
}