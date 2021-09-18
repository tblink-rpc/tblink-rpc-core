/*
 * BfmReqRsp.h
 *
 *  Created on: Sep 15, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IEndpoint.h"

class BfmReqRsp {
public:
	BfmReqRsp();

	virtual ~BfmReqRsp();

	void req(int32_t v);

	void rsp(int32_t v);

	void req_f(const std::function<void(int32_t)>	&f) {
		m_req_f = f;
	}

	void rsp_f(const std::function<void(int32_t)>	&f) {
		m_rsp_f = f;
	}

	static tblink_rpc_core::IInterfaceType *registerType(
			tblink_rpc_core::IEndpoint *ep);

	static BfmReqRsp *mkInst(
			tblink_rpc_core::IEndpoint 	*ep,
			const std::string			&inst_name,
			bool						is_mirror);

private:
	void invoke(
			tblink_rpc_core::IInterfaceInst		*ifinst,
			tblink_rpc_core::IMethodType 		*method_t,
			intptr_t							call_id,
			tblink_rpc_core::IParamValVec		*params);

private:
	tblink_rpc_core::IInterfaceInst				*m_ifinst;
	tblink_rpc_core::IMethodType				*m_req_mt;
	tblink_rpc_core::IMethodType				*m_rsp_mt;

	std::function<void(int32_t)>				m_req_f;
	std::function<void(int32_t)>				m_rsp_f;


};

