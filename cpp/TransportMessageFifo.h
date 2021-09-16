/*
 * FifoMessageTransport.h
 *
 *  Created on: Aug 13, 2021
 *      Author: mballance
 */

#pragma once
#include <vector>
#include "tblink_rpc/ITransport.h"
#include "TransportMessageFifoEP.h"

namespace tblink_rpc_core {

class TransportMessageFifo;
typedef std::unique_ptr<TransportMessageFifo> FifoMessageTransportUP;
class TransportMessageFifo {
public:
	TransportMessageFifo(bool is_blocking=false);

	TransportMessageFifo(
			TransportMessageFifoEP		*ep0,
			TransportMessageFifoEP		*ep1,
			bool 						is_blocking=false);

	virtual ~TransportMessageFifo();

	virtual TransportMessageFifoEP *ep(uint32_t id);

	virtual int32_t send_req(
			uint32_t				ep_id,
			const std::string		&method,
			intptr_t				req_id,
			IParamValMap			*params);

	virtual int32_t send_notify(
			uint32_t				ep_id,
			const std::string		&method,
			IParamValMap			*params);

	virtual int32_t send_rsp(
			uint32_t			ep_id,
			intptr_t			id,
			IParamValMap		*result,
			IParamValMap		*error);

private:
	bool							m_is_blocking;
	TransportMessageFifoEPUP		m_ep0;
	TransportMessageFifoEPUP		m_ep1;

};

} /* namespace tblink_rpc_core */

