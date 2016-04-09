
#ifndef FIFO_FIR_STIMULUS
#define FIFO_FIR_STIMULUS

#include <systemc.h>

SC_MODULE(Stimulus) {

	// Port declaration
	sc_in_clk 		iclk;
	sc_out< bool >	Reset;
	sc_port<sc_fifo_out_if<double> > orig_in;
	sc_port<sc_fifo_out_if<double> > data_in;


	// Channel, submodules declaration

	// Constructor declaration
	SC_HAS_PROCESS(Stimulus);
	Stimulus(sc_module_name mn);

	// Process declaration
	void process_thread(void);

	// Helper declartion

	// Private data


	

};

#endif

