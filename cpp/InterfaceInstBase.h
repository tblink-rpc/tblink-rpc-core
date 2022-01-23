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
class InterfaceInstBase;
typedef std::unique_ptr<InterfaceInstBase> InterfaceInstUP;
class InterfaceInstBase : public IInterfaceInst {
public:
	InterfaceInstBase(
			IEndpoint						*endpoint,
			IInterfaceType					*type,
			const std::string				&inst_name,
			bool							is_mirror);

	InterfaceInstBase(
			IEndpoint						*endpoint,
			IInterfaceType					*type,
			const std::string				&inst_name,
			bool							is_mirror,
			IInterfaceImpl					*impl);

	virtual ~InterfaceInstBase();

	virtual IEndpoint *endpoint() const override {
		return m_endpoint;
	}

	virtual IInterfaceType *type() override { return m_type; }

	virtual bool is_mirror() override { return m_is_mirror; }

	virtual void setImpl(IInterfaceImpl *impl) override;

	virtual const std::string &name() override {
		return m_inst_name;
	}

	void invoke_req(
			IMethodType				*method,
			IParamValVec			*params,
			const invoke_rsp_f		&response_f);

	/** Must be implemented by sub-types
	virtual int32_t invoke(
			IMethodType									*method,
			IParamValVec								*params,
			const invoke_rsp_f							&completion_f) override;
	 */

	virtual void invoke_rsp(
			intptr_t									call_id,
			IParamVal									*ret) override;

	virtual void invoke_nb_rsp(
			intptr_t									id,
			IParamValMap								*result,
			IParamValMap								*error);

	/**
	 * Response to a remote call
	 */
	void invoke_b_rsp(
			intptr_t									call_id,
			IParamVal									*ret
			);

	virtual IParamValBool *mkValBool(bool val) override;

	virtual IParamValInt *mkValIntU(uint64_t val, int32_t width) override;

	virtual IParamValInt *mkValIntS(int64_t val, int32_t width) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVec *mkValVec() override;

protected:
	IEndpoint									*m_endpoint;
	IInterfaceType								*m_type;
	std::string									m_inst_name;
	bool										m_is_mirror;
	IInterfaceImplUP							m_impl;
	intptr_t									m_call_id;

	std::unordered_map<intptr_t,invoke_rsp_f>	m_outbound_invoke_m;

	std::unordered_map<intptr_t,invoke_rsp_f>	m_invoke_m;

};

} /* namespace tblink_rpc_core */

