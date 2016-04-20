#ifndef __INTERCHANGE_H__
#define __INTERCHANGE_H__

#include <systemc.h>

// Using direct sub-Module coding style
class interchange : public sc_module	// inherit sc_module class
{
public:
	// Port declaration
	sc_in_clk         clk;
	sc_out<bool>      LD;
	sc_out<bool>      RW;
	sc_out<unsigned>  X;
	sc_out<unsigned>  Y;
	sc_inout_rv<32>   D;

	// Constructor declaration
	SC_HAS_PROCESS(interchange);
	interchange(sc_module_name name, int size);

	// Processes
	void main(void);
	enum operation { WRITE = false, READ = true };
private:
	int **local_mem;
	int size = 8;
};

#endif