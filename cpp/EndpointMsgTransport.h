/*
 * EndpointMsgTransport.h
 *
 *  Created on: Dec 8, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/ITransport.h"
#include "EndpointMsgBase.h"

namespace tblink_rpc_core {

class EndpointMsgTransport : public EndpointMsgBase {
public:
	EndpointMsgTransport(ITransport *transport);

	virtual ~EndpointMsgTransport();

	/**
	 * Poll, waiting for a message to be received.
	 * Returns -1 on transport error.
	 */
	virtual int32_t process_one_message() override;

	virtual int32_t send_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMap			*params) override;

	virtual int32_t send_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error) override;

	virtual IParamValBool *mkValBool(bool val) override;

	virtual IParamValInt *mkValIntU(uint64_t val, int32_t width) override;

	virtual IParamValInt *mkValIntS(int64_t val, int32_t width) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVec *mkValVec() override;

private:
	ITransport							*m_transport;

};

} /* namespace tblink_rpc_core */

