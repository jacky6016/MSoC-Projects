#ifndef FIFO_FIR_EX
#define FIFO_FIR_EX
#include <systemc.h>

SC_MODULE(sc_fifo_ex) {

	// Port declaration
	sc_in_clk    iclk;   // postive edge triggerd
	sc_port<sc_fifo_in_if<double> >  data_in;
	sc_port<sc_fifo_out_if<double> > data_out;
	sc_out <double> d_in;
	// data for fir calculation
	double*     m_pipe;     // data pipe
	double*     m_coeff;    // array of coefficients
	unsigned m_taps;     // number of coefficients
	unsigned m_tap;      // current tap
	char* m_cfg_filename;

	// Constructor
	SC_HAS_PROCESS(sc_fifo_ex);	
	sc_fifo_ex(sc_module_name _name, char* _cfg_filename = "fifo_fir.cfg");

	// Destructor
	~sc_fifo_ex() {
		if (m_taps) {
			delete[] m_coeff;
			delete[] m_pipe;
		}//endif
	}//end ~fifo_fir()

	// Processes
	void sc_fifo_ex_thread(void);
};

#endif