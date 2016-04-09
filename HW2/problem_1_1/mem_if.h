#ifndef __MEM_IF_H__
#define __MEM_IF_H__
#include <systemc.h>

class mem_if : virtual public sc_interface {
public:
	virtual void direct_read(int** block) = 0;
	virtual void direct_write(int** block) = 0;
};

#endif