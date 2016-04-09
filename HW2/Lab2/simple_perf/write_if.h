#ifndef WRITE_IF_H
#define WRITE_IF_H

#include <systemc.h>

class write_if : virtual public sc_interface
{
public:
    virtual void write(char) = 0;
	// Add new function 'isFull for blocking access'
	virtual bool isFull() = 0;
    virtual void reset() = 0;
};

#endif