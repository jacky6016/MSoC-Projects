

#include <systemc.h>
#include "Counter.h"
#include "Stimulus.h"
#include "Display.h"

int sc_main( int argc, char** argv ) {

    /*-	Signal Declaration	-*/
    sc_time	        SIM_TIME( 500, SC_NS );
    sc_clock        clk ("clk", sc_time(10,SC_NS) );
    sc_signal<int>	Value;
    sc_signal<bool>	Enable;
    sc_signal<bool>	UpDown;
    sc_signal<bool> Reset;

    /*-	Instantiation and Net Connection	-*/
    Counter iCounter( "iCounter" );
    iCounter.iclk( clk );
	iCounter.Reset(Reset);  // true for reset
	iCounter.Enable(Enable); // true for counting
	iCounter.UpDown(UpDown); // ture for count up
	iCounter.Value(Value);

    Stimulus iStimulus( "iStimulus"	);
    iStimulus.iclk( clk );
	iStimulus.Reset(Reset);
	iStimulus.Enable(Enable);
	iStimulus.UpDown(UpDown);

	/* Trace waveform */ 
	sc_trace_file *fp;                            // Declare FilePointer fp
	fp = sc_create_vcd_trace_file("wave");        // Open the VCD file, create wave.vcd file 

	sc_trace(fp, clk, "clk");                     // Add signals to trace file
	sc_trace(fp, Reset, "Reset");
	sc_trace(fp, Enable, "Enable");
	sc_trace(fp, UpDown, "UpDown");
	sc_trace(fp, Value, "Value");
    /*-	Run Simulation	-*/
    sc_start( SIM_TIME );
	sc_close_vcd_trace_file(fp);                  // close(fp)
    /*-	Clean Up	-*/	
    return 0;

}



