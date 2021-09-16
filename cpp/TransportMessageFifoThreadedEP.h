/*
 * TransportMessageFifoThreadedEP.h
 *
 *  Created on: Sep 15, 2021
 *      Author: mballance
 */

#pragma once
#include "TransportMessageFifoEP.h"
#include "pthread.h"

namespace tblink_rpc_core {

class TransportMessageFifoThreadedEP : public TransportMessageFifoEP {
public:
	TransportMessageFifoThreadedEP();

	virtual ~TransportMessageFifoThreadedEP();

	/**
	 * Polls, waiting for a message. Returns -1 in case of failure.
	 */
	virtual int32_t process_one_message() override;

protected:

	virtual int32_t recv_req(
			const std::string		&method,
			intptr_t				req_id,
			IParamValMap			*params);

	virtual int32_t recv_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error);

private:
	pthread_mutex_t					m_recv_mut;
	pthread_cond_t					m_recv_cond;
	uint32_t						m_recv_cnt;

};

} /* namespace tblink_rpc_core */

