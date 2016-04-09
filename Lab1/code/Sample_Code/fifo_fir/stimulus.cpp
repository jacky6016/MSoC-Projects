#include "Stimulus.h"

Stimulus::Stimulus(sc_module_name mn)
	: sc_module(mn)
{
	SC_THREAD(process_thread);
	sensitive << iclk.pos();
}

void Stimulus::process_thread(void)
{	// initialization phase
	int i;
	Enable.write(false);
	Reset.write(false);
	UpDown.write(false);
	wait();

	sc_time DELAY(1, SC_NS);
	unsigned PTS = 20;
	for (unsigned t = 0; t<PTS; t++) {
		double data = 0.0;
		if (t >= 8 && t <= 12) data = 1.0; // impulse
		orig_in.write(data);
		data_in.write(data);
		wait(DELAY);
	}//endfor

	// running phase
	Reset.write(true);
	wait();
	Reset.write(false);
	wait();

	Enable.write(true);
	UpDown.write(true);

	for (i = 0; i<20; i++)
		wait();

	Enable.write(false);

	for (i = 0; i<4; i++)
		wait();

	/*- Add count down test here -*/
	Enable.write(true);
	UpDown.write(false);

	for (i = 0; i<20; i++)
		wait();

	for (;;)	// trap for keeping process alive
		wait();

}