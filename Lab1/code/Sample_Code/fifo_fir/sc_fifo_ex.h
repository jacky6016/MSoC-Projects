#ifndef FIFO_FIR_EX
#define FIFO_FIR_EX
#include <systemc.h>

SC_MODULE(sc_fifo_ex) {

	// Port declaration
	sc_in_clk    iclk;   // postive edge triggerd
	sc_out<bool> Reset;
	sc_port<sc_fifo_in_if<double> >  data_in;
	sc_port<sc_fifo_out_if<double> > data_out;

	// Channel, submodules declaration

	// Constructor declaration
	SC_CTOR(sc_fifo_ex) {             // registration the constructor of "Counter"
		SC_METHOD(process_method);    // registration entry() as event handler
		dont_initialize();            // method entry() is not made runnable during initialization
		sensitive << iclk.pos();      // entry() is triggered at positive edge of Clock
		v = 0;                        // give counter initial value
	}

	// Process declaration
	void process_method(void);

	// Helper declartion

	// Private data
	int  v;


};

#endif