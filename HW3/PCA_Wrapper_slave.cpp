#include "PCA_Wrapper_slave.h"

simple_bus_status PCA_Wrapper_slave::read(int *data, unsigned int address){

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

simple_bus_status PCA_Wrapper_slave::write(int *data, unsigned int address){

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