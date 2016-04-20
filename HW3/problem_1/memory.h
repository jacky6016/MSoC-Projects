#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <systemc.h>
#include "mem_if.h"

// Inherit the mem_if interface
class memory : public sc_channel, public mem_if
{
public:
	SC_HAS_PROCESS(memory);
	memory(sc_module_name name, int size);
	~memory();
	void word_read(unsigned x, unsigned y, int& d);
	void word_write(unsigned x, unsigned y, int d);
	void main(void);

	void notify() { m_interrupt.notify(); }
	const sc_event& default_event() const { return m_interrupt; }

private:
	int **local_mem; 
	int size = 8; // default 2d array size
	void printLocalMem(int size, char *state);
	sc_event m_interrupt; 
};

#endif