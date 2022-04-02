/*
 * EndpointLoopback.h
 *
 *  Created on: Nov 26, 2021
 *      Author: mballance
 */

#pragma once
#include "EndpointMsgBase.h"
#include "tblink_rpc/IEndpointLoopback.h"

namespace tblink_rpc_core {

class EndpointLoopback;
using EndpointLoopbackUP=std::unique_ptr<EndpointLoopback>;

using time_cb_f=std::function<void()>;

class EndpointLoopback : public EndpointMsgBase, public virtual IEndpointLoopback {
public:
	EndpointLoopback();

	EndpointLoopback(EndpointLoopback *peer);

	virtual ~EndpointLoopback();

	virtual int32_t process_one_message() override;

	virtual int32_t send_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMap			*params) override;

	int32_t peer_recv_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMap			*params);

	virtual int32_t send_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error) override;

	int32_t peer_recv_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error);

	virtual IEndpoint *peer() override { return m_peer; }

protected:
	bool													m_primary;
	EndpointLoopback										*m_peer;
	EndpointLoopbackUP										m_peer_u;

};

} /* namespace tblink_rpc_core */

