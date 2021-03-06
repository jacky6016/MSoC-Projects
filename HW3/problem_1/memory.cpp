#include <systemc.h>
#include <time.h>
#include <iomanip>
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

void memory::word_read(unsigned x, unsigned y, int& d)
{
	//wait(m_interrupt);
	if (x < size && y < size)
		d = local_mem[x][y];
	else
		cout << "Trying to access out-of-bound memory";
}

void memory::word_write(unsigned x, unsigned y, int d)
{
	if (int(x) < size && int(y) < size)
		local_mem[x][y] = d;
	else
		cout << "Trying to access out-of-bound memory";
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

void memory::main(void)
{
	wait(m_interrupt);
}