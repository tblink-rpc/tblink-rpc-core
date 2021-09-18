/*
 * JsonInterfaceInst.h
 *
 *  Created on: Jul 13, 2021
 *      Author: mballance
 */

#pragma once
#include <unordered_map>
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IInterfaceInst.h"

namespace tblink_rpc_core {

class EndpointMsgTransport;
class InterfaceInst;
typedef std::unique_ptr<InterfaceInst> InterfaceInstUP;
class InterfaceInst : public IInterfaceInst {
public:
	InterfaceInst(
			EndpointMsgTransport			*endpoint,
			IInterfaceType					*type,
			const std::string				&inst_name,
			bool							is_mirror);

	InterfaceInst(
			EndpointMsgTransport			*endpoint,
			IInterfaceType					*type,
			const std::string				&inst_name,
			bool							is_mirror,
			const invoke_req_f				&req_f);

	virtual ~InterfaceInst();

	virtual IInterfaceType *type() override { return m_type; }

	virtual bool is_mirror() { return m_is_mirror; }

	virtual void set_invoke_req_f(const invoke_req_f &req_f) override {
		m_req_f = req_f;
	}

	virtual const std::string &name() override {
		return m_inst_name;
	}

	void invoke_req(
			IMethodType				*method,
			IParamValVec			*params,
			const invoke_rsp_f		&response_f);

	virtual IParamVal *invoke(
			IMethodType									*method,
			IParamValVec								*params) override;

	virtual int32_t invoke_nb(
			IMethodType									*method,
			IParamValVec								*params,
			const invoke_rsp_f							&completion_f) override;

	virtual void invoke_rsp(
			intptr_t									call_id,
			IParamVal									*ret) override;

	virtual void invoke_nb_rsp(
			intptr_t									id,
			IParamValMap								*result,
			IParamValMap								*error);

	virtual IParamValBool *mkValBool(bool val) override;

	virtual IParamValInt *mkValIntU(uint64_t val, int32_t width) override;

	virtual IParamValInt *mkValIntS(int64_t val, int32_t width) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVec *mkValVec() override;

private:
	EndpointMsgTransport						*m_endpoint;
	IInterfaceType								*m_type;
	std::string									m_inst_name;
	bool										m_is_mirror;
	invoke_req_f								m_req_f;
	intptr_t									m_call_id;
	std::unordered_map<intptr_t,invoke_rsp_f>	m_invoke_m;

};

} /* namespace tblink_rpc_core */

