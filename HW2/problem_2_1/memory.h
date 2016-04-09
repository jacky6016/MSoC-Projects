#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <systemc.h>

// Inherit the mem_if interface
class memory : public sc_module
{
public:
	sc_in_clk        clk;
	sc_in<bool>      LD;
	sc_in<bool>      RW;
	sc_in<unsigned>  X;
	sc_in<unsigned>  Y;
	sc_inout_rv<32>  D;

	SC_HAS_PROCESS(memory);
	memory(sc_module_name name, int size);
	~memory();
	void main(void);

private:
	int **local_mem; 
	int size = 8; // default 2d array size
	void printLocalMem(int size, char *state);
};

#endif