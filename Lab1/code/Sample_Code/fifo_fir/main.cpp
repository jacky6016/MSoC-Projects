//BEGIN main.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//See fifo_fir.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <iostream>
using std::cout;
using std::endl;

#include <systemc.h>
#include "sc_fifo_ex.h"
#include "stimulus.h"
#include "result.h"

unsigned errors = 0;
char* simulation_name = "fifo_fir";

int sc_main(int argc, char* argv[]) {
	//sc_set_time_resolution(1,SC_PS);
	//sc_set_default_time_unit(1,SC_NS);

	/*-	Signal Declaration	-*/
	sc_time	        SIM_TIME(500, SC_NS);
	sc_clock        clk("clk", sc_time(10, SC_NS));
	sc_signal<bool> Reset;
	sc_fifo<double> orig_in;
	sc_fifo<double> data_in;
	sc_fifo<double> data_out;

	/*-	Instantiation and Net Connection	-*/
	sc_fifo_ex sc_fifo_ex_i("sc_fifo_ex_i");
	sc_fifo_ex_i.iclk(clk);
	sc_fifo_ex_i.Reset(Reset);  // true for reset


	Stimulus iStimulus("iStimulus");
	iStimulus.iclk(clk);
	iStimulus.Reset(Reset);

	Results iResults("iResults");
	iResults.iclk(clk);
	iResults.Reset(Reset);

	/* Trace waveform */
	sc_trace_file *fp;                            // Declare FilePointer fp
	fp = sc_create_vcd_trace_file("wave");        // Open the VCD file, create wave.vcd file 

	sc_trace(fp, clk, "clk");                     // Add signals to trace file
	sc_trace(fp, Reset, "Reset");

	
	cout << "INFO: Starting " << simulation_name << " simulation" << endl;
	if (errors == 0) sc_start();

	//	sc_close_vcd_trace_file(fp);                  // close(fp)

	cout << "INFO: Exiting " << simulation_name << " simulation" << endl;
	cout << "INFO: Simulation " << simulation_name
		<< " " << (errors ? "FAILED" : "PASSED")
		<< " with " << errors << " errors"
		<< endl;
	return errors ? 1 : 0;

	/*-	Run Simulation	-*/
	sc_start(SIM_TIME);
	sc_close_vcd_trace_file(fp);                  // close(fp)
	/*-	Clean Up	-*/
	return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: main.cpp,v 1.1 2003/11/24 17:21:26 dcblack Exp $
