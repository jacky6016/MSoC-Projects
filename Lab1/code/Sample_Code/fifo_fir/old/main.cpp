//BEGIN main.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//See fifo_fir.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <iostream>
using std::cout;
using std::endl;

#include <systemc.h>
#include "fifo_fir.h"

unsigned errors = 0;
char* simulation_name = "fifo_fir";

int sc_main(int argc, char* argv[]) {
    //sc_set_time_resolution(1,SC_PS);
    //sc_set_default_time_unit(1,SC_NS);
    fifo_fir sc_fifo_ex_i("sc_fifo_ex_i");
	
	/* Trace waveform */
//	sc_trace_file *fp;                            // Declare FilePointer fp
//	fp = sc_create_vcd_trace_file("wave");        // Open the VCD file, create wave.vcd file 
   
/*	sc_trace(fp, clk, "clk");                     // Add signals to trace file
	sc_trace(fp, Reset, "Reset");
	sc_trace(fp, Enable, "Enable");
	sc_trace(fp, UpDown, "UpDown");
	sc_trace(fp, Value, "Value");
	*/
	cout << "INFO: Starting "<<simulation_name<<" simulation" << endl;
    if (errors == 0) sc_start();
	
//	sc_close_vcd_trace_file(fp);                  // close(fp)
    
	cout << "INFO: Exiting "<<simulation_name<<" simulation" << endl;
    cout << "INFO: Simulation " << simulation_name
        << " " << (errors?"FAILED":"PASSED")
        << " with " << errors << " errors"
        << endl;
    return errors?1:0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: main.cpp,v 1.1 2003/11/24 17:21:26 dcblack Exp $
