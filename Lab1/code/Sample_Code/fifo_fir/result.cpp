#include "result.h"
#include <iostream>

using std::cout;
using std::endl;

Results::Results(sc_module_name mn)
	: sc_module(mn), old_value(0)
{	// process registration and sensitivity
	SC_METHOD(process_method);
	sensitive << Value;
}

void Results::process_method(void)
{
	if (old_value != Value.read())
	{
		cout << "Updated to " << Value.read() << " at " << sc_time_stamp() << endl;
		old_value = Value.read();
	}
	else
		cout << "Hold old value" << endl;	// Please observe if this statement will be executed

	for (unsigned i = 0;; i++) {
		double data = orig_in.read();
		double result = data_out.read();
		cout << "DATA: "
			<< "[" << setw(2) << i << "]"
			<< "= " << setw(9) << fixed << setprecision(5) << data
			<< " " << setw(9) << fixed << setprecision(5) << result
			<< endl;
	}


}
