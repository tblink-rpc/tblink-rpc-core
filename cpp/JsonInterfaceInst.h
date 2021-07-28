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

	virtual const std::string &name() override {
		return m_inst_name;
	}

	void invoke_req(
			IMethodType				*method,
			IParamValVectorSP		params,
			const invoke_rsp_f		&response_f);

	virtual int32_t invoke(
			IMethodType									*method,
			IParamValVectorSP							params,
			const invoke_rsp_f							&completion_f) override;

	virtual IParamValSP invoke_nb(
			IMethodType									*method,
			IParamValVectorSP							params) override;

	virtual void invoke_rsp(
			intptr_t									call_id,
			IParamValSP									ret) override;

	virtual IParamValBoolSP mkValBool(bool val) override;

	virtual IParamValIntSP mkValIntU(uint64_t val) override;

	virtual IParamValIntSP mkValIntS(int64_t val) override;

	virtual IParamValMapSP mkValMap() override;

	virtual IParamValStrSP mkValStr(const std::string &val) override;

	virtual IParamValVectorSP mkVector() override;

private:
	JsonRpcEndpoint						*m_endpoint;
	IInterfaceType						*m_type;
	std::string							m_inst_name;
	invoke_req_f						m_req_f;
	intptr_t							m_call_id;
	std::map<intptr_t,invoke_rsp_f>		m_invoke_m;

};

} /* namespace tblink_rpc_core */

