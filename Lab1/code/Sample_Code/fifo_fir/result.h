
#ifndef	FIFO_FIR_RESULTS
#define FIFO_FIR_RESULTS

#include <systemc.h>

SC_MODULE(Results) {

	// Port declaration
	sc_port<sc_fifo_in_if<double> > orig_in;;
	sc_port<sc_fifo_in_if<double> > data_out;
	// Channel, submodules declaration

	// Constructor declaration
	SC_HAS_PROCESS(Results);
	Results(sc_module_name mn);

	// Process declaration
	void process_method(void);

	// Helper declartion

	// Private data
	int old_value;
};

#endif

