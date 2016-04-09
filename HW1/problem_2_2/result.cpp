#include "result.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

using std::cout;
using std::endl;
using std::setw;
using std::fixed;
using std::setprecision;

Results::Results(sc_module_name mn)
	: sc_module(mn)
{	// process registration and sensitivity
	SC_THREAD(process_thread);
	sensitive << iclk.pos();
}

void Results::process_thread(void)
{
	for (unsigned i = 0;; i++) {
		double data = orig_in->read();
		double result = data_out->read();
		o_in.write(data);
		d_out.write(result);
		cout << "DATA: "
			<< "[" << setw(2) << i << "]"
			<< "= " << setw(9) << fixed << setprecision(5) << data
			<< " " << setw(9) << fixed << setprecision(5) << result
			<< endl;
	}

}
