#include "slave_wrapper.h"

slave_wrapper::slave_wrapper(sc_module_name name_, unsigned int start_addr, unsigned int end_addr, unsigned int nr_wait_states) :
sc_module(name_), m_start_addr(start_addr), m_end_addr(end_addr), m_nr_wait_states(nr_wait_states), m_wait_count(-1) {
	SC_METHOD(wait_loop);
	sensitive << clk.pos();
};

simple_bus_status slave_wrapper::read(int *data, unsigned int address){

	if (m_wait_count < 0){
		LD->write(true);
		RW->write(true);
		X->write(((address - m_start_addr) / 4) / 8);
		Y->write(((address - m_start_addr) / 4) % 8);

		m_wait_count = m_nr_wait_states;
		return SIMPLE_BUS_WAIT;
	}

	if (m_wait_count == 0){
		LD->write(false);
		*data = D->read().to_long();

		return SIMPLE_BUS_OK;
	}
	return SIMPLE_BUS_WAIT;
}

simple_bus_status slave_wrapper::write(int *data, unsigned int address){

	if (m_wait_count < 0){
		LD->write(true);
		RW->write(false);
		X->write(((address - m_start_addr) / 4) / 8);
		Y->write(((address - m_start_addr) / 4) % 8);
		D->write(*data);

		m_wait_count = m_nr_wait_states;
		return SIMPLE_BUS_WAIT;
	}
	if (m_wait_count == 0){
		LD->write(false);
		return SIMPLE_BUS_OK;
	}
	return SIMPLE_BUS_WAIT;
}