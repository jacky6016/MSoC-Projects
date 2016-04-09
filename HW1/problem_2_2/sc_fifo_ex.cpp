#include "sc_fifo_ex.h"
extern unsigned errors;

// Constructor
sc_fifo_ex::sc_fifo_ex(sc_module_name _name, char* _cfg_filename) :
sc_module(_name),
m_cfg_filename(_cfg_filename),
m_taps(0),
m_tap(0)
{
	SC_THREAD(sc_fifo_ex_thread);
	sensitive << iclk.pos();
	// Read coefficients from configuration file and initialize pipe to zero
	ifstream cfg_file(m_cfg_filename);
	if (!cfg_file) {
		cerr << "ERROR: Problem opening " << m_cfg_filename << " for input." << endl;
		errors++;
		return;
	}//endif
	cfg_file >> m_taps;
	if (m_taps < 1) {
		cerr << "ERROR: Number of taps must be greater than zero!" << endl;
		errors++;
		return;
	}//endif
	// Allocate memory
	m_pipe = new double[m_taps];
	m_coeff = new double[m_taps];
	for (unsigned tap = 0; tap != m_taps; tap++) {
		if (cfg_file.eof()) {
			cerr << "ERROR: Too few coefficients when reading [" << tap << "]." << endl;
			errors++;
			return;
		}//endif
		cfg_file >> m_coeff[tap];
		m_pipe[tap] = 0;
	}//endfor
}//end fifo_fir()

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Processes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void sc_fifo_ex::sc_fifo_ex_thread(void) {
	for (;;) {
		unsigned coeff = (m_taps - 1 - m_tap); // used to index coeffiecients
		// read next piece of data
		double data = data_in->read();
		d_in.write(data);
		m_pipe[m_tap++] = data;
		if (m_tap == m_taps) m_tap = 0; // wrap
		double result = 0;
		for (unsigned tap = 0; tap != m_taps; tap++, coeff++) {
			if (coeff == m_taps) coeff = 0; // wrap
			result += m_coeff[coeff] * m_pipe[tap];
		}//endfor

		data_out->write(result);
		wait(); 
	}//endforever
}//end sc_fifo_ex_thread()