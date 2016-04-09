#ifndef __INTERCHANGE_H__
#define __INTERCHANGE_H__

#include <systemc.h>
#include "mem_if.h"

// Using direct sub-Module coding style
class interchange : public sc_module	// inherit sc_module class
{
public:
	// Port declaration
	sc_port<mem_if> mem_port;	

	// Constructor declaration
	SC_HAS_PROCESS(interchange);
	interchange(sc_module_name name, int size);

	// Processes
	void main(void);

private:
	int size = 8;
};

#endif