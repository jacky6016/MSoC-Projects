
#ifndef	FIFO_FIR_RESULTS
#define FIFO_FIR_RESULTS

#include <systemc.h>

SC_MODULE(Results) {

	// Port declaration
	sc_in_clk    iclk;
	sc_port<sc_fifo_in_if<double> > orig_in;
	sc_port<sc_fifo_in_if<double> > data_out;
	sc_out <double> o_in, d_out;
	// Channel, submodules declaration

	// Constructor declaration
	SC_HAS_PROCESS(Results);
	Results(sc_module_name mn);

	// Process declaration
	void process_thread(void);

};

#endif

