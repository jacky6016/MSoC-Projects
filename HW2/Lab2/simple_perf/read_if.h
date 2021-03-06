#ifndef READ_IF_H
#define READ_IF_H

#include <systemc.h>

class read_if : virtual public sc_interface
{
public:
    virtual void read(char &) = 0;
	// Add new function 'isEmpty for blocking access'
	virtual bool isEmpty() = 0;
    virtual int num_available() = 0;
};

#endif