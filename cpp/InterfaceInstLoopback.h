/*
 * InterfaceInstLoopback.h
 *
 *  Created on: Nov 29, 2021
 *      Author: mballance
 */

#pragma once
#include "InterfaceInstBase.h"
#include "tblink_rpc/IInterfaceType.h"

namespace tblink_rpc_core {

class EndpointLoopback;

class InterfaceInstLoopback : public InterfaceInstBase {
public:
	InterfaceInstLoopback(
			EndpointLoopback		*endpoint,
			IInterfaceType			*type,
			const std::string		&inst_name,
			bool					is_mirror);

	InterfaceInstLoopback(
			EndpointLoopback		*endpoint,
			IInterfaceType			*type,
			const std::string		&inst_name,
			bool					is_mirror,
			const invoke_req_f		&req_f);

	virtual ~InterfaceInstLoopback();

	InterfaceInstLoopback *peer() const { return m_peer; }

	void peer(InterfaceInstLoopback *p);

	virtual IParamVal *invoke(
			IMethodType									*method,
			IParamValVec								*params) override;

	virtual int32_t invoke_nb(
			IMethodType									*method,
			IParamValVec								*params,
			const invoke_rsp_f							&completion_f) override;

	int32_t req_invoke_nb(
			IMethodType									*method,
			intptr_t									call_id,
			IParamValVec								*params);

	void rsp_invoke(
			intptr_t									call_id,
			IParamVal									*retval);

private:
	InterfaceInstLoopback								*m_peer;
	std::unordered_map<IMethodType*,IMethodType*>		m_method_m;
	std::unordered_map<intptr_t,invoke_rsp_f>			m_rsp_m;
	intptr_t											m_call_id;

};

} /* namespace tblink_rpc_core */

