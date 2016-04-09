#include <time.h>
#include <iomanip>
#include <systemc.h>
#include "memory.h"

memory::memory(sc_module_name name, int size) : sc_channel(name)
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

void memory::direct_read(int** block)
{
	cout << "Reading data in memory." << endl;
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			block[i][j] = local_mem[i][j];
		}
	}
}

void memory::direct_write(int** block)
{
	cout << "Writing data in memory." << endl;
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			local_mem[i][j] = block[i][j];
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