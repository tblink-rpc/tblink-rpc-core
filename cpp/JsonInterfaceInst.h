/*
 * JsonInterfaceInst.h
 *
 *  Created on: Jul 13, 2021
 *      Author: mballance
 */

#pragma once
#include <map>
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IInterfaceInst.h"

namespace tblink_rpc_core {

class JsonRpcEndpoint;
class JsonInterfaceInst;
typedef std::unique_ptr<JsonInterfaceInst> JsonInterfaceInstUP;
typedef std::shared_ptr<JsonInterfaceInst> JsonInterfaceInstSP;
class JsonInterfaceInst : public IInterfaceInst {
public:
	JsonInterfaceInst(
			JsonRpcEndpoint			*endpoint,
			IInterfaceType			*type,
			const std::string		&inst_name);

	JsonInterfaceInst(
			JsonRpcEndpoint			*endpoint,
			IInterfaceType			*type,
			const std::string		&inst_name,
			const invoke_req_f		&req_f);

	virtual ~JsonInterfaceInst();

	virtual IInterfaceType *type() override { return m_type; }

	virtual void set_invoke_req_f(const invoke_req_f &req_f) override {
		m_req_f = req_f;
	}

	virtual const std::string &name() override {
		return m_inst_name;
	}

	void invoke_req(
			IMethodType				*method,
			IParamValVector			*params,
			const invoke_rsp_f		&response_f);

	virtual int32_t invoke(
			IMethodType									*method,
			IParamValVector								*params,
			const invoke_rsp_f							&completion_f) override;

	virtual IParamVal *invoke_nb(
			IMethodType									*method,
			IParamValVector								*params) override;

	virtual void invoke_rsp(
			intptr_t									call_id,
			IParamVal									*ret) override;

	virtual IParamValBool *mkValBool(bool val) override;

	virtual IParamValInt *mkValIntU(uint64_t val) override;

	virtual IParamValInt *mkValIntS(int64_t val) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVector *mkVector() override;

private:
	JsonRpcEndpoint						*m_endpoint;
	IInterfaceType						*m_type;
	std::string							m_inst_name;
	invoke_req_f						m_req_f;
	intptr_t							m_call_id;
	std::map<intptr_t,invoke_rsp_f>		m_invoke_m;

};

} /* namespace tblink_rpc_core */

