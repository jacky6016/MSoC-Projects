#ifndef __MEM_IF_H__
#define __MEM_IF_H__
#include <systemc.h>

class mem_if : virtual public sc_interface {
public:
	virtual void word_read(unsigned x, unsigned y, int& d) = 0;
	virtual void word_write(unsigned x, unsigned y, int d) = 0;

};

#endif