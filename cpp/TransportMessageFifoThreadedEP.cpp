/*
 * TransportMessageFifoThreadedEP.cpp
 *
 *  Created on: Sep 15, 2021
 *      Author: mballance
 */

#include "TransportMessageFifoThreadedEP.h"

namespace tblink_rpc_core {

TransportMessageFifoThreadedEP::TransportMessageFifoThreadedEP() :
	TransportMessageFifoEP(0, 0), m_recv_cnt(0) {
	pthread_mutex_init(&m_recv_mut, 0);
	pthread_cond_init(&m_recv_cond, 0);

}

TransportMessageFifoThreadedEP::~TransportMessageFifoThreadedEP() {
	// TODO Auto-generated destructor stub
}


int32_t TransportMessageFifoThreadedEP::process_one_message() {
	pthread_mutex_lock(&m_recv_mut);
	if (!m_recv_cnt) {
		pthread_cond_wait(&m_recv_cond, &m_recv_mut);
	}
	m_recv_cnt = 0;
	pthread_mutex_unlock(&m_recv_mut);
	return 0;
}


int32_t TransportMessageFifoThreadedEP::recv_req(
			const std::string		&method,
			intptr_t				req_id,
			IParamValMap			*params) {
	int32_t ret = TransportMessageFifoEP::recv_req(method, req_id, params);
	pthread_mutex_lock(&m_recv_mut);
	m_recv_cnt += 1;
	pthread_cond_signal(&m_recv_cond);
	pthread_mutex_unlock(&m_recv_mut);
	return ret;
}

int32_t TransportMessageFifoThreadedEP::recv_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error) {
	int32_t ret = TransportMessageFifoEP::recv_rsp(id, result, error);
	pthread_mutex_lock(&m_recv_mut);
	m_recv_cnt += 1;
	pthread_cond_signal(&m_recv_cond);
	pthread_mutex_unlock(&m_recv_mut);

	return ret;
}

} /* namespace tblink_rpc_core */
