#include "sc_fifo_ex.h"

void sc_fifo_ex::process_method(void) {
	for (;;) {
		unsigned coeff = (m_taps - 1 - m_tap); // used to index coeffiecients
		// read next piece of data
		double data = data_in.read();
		m_pipe[m_tap++] = data;
		if (m_tap == m_taps) m_tap = 0; // wrap
		double result = 0;
		for (unsigned tap = 0; tap != m_taps; tap++, coeff++) {
			if (coeff == m_taps) coeff = 0; // wrap
			result += m_coeff[coeff] * m_pipe[tap];
		}//endfor
		data_out.write(result);
	}//endforever
}//end sc_fifo_ex_thread()