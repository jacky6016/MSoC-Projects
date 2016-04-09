#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <systemc.h>
#include "mem_if.h"

// Inherit the mem_if interface
class memory : public sc_channel, public mem_if
{
public:
	memory(sc_module_name name, int size);
	~memory();
	void direct_read(int** block);
	void direct_write(int** block);

private:
	int **local_mem; 
	int size = 8; // default 2d array size
	void printLocalMem(int size, char *state);
};

#endif