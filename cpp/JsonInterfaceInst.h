/*
 * JsonInterfaceInst.h
 *
 *  Created on: Jul 13, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IInterfaceInst.h"

namespace tblink_rpc_core {

class JsonInterfaceInst : public IInterfaceInst {
public:
	JsonInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			const invoke_req_f		&req_f);

	virtual ~JsonInterfaceInst();

	virtual IInterfaceType *type() override { return m_type; }

	virtual int32_t invoke(
			IMethodType									*method,
			IParamValVectorSP							params,
			const invoke_rsp_f							&completion_f) override;

	virtual IParamValSP invoke_nb(
			IMethodType									*method,
			IParamValVectorSP							params) override;

	virtual void respond(
			intptr_t									call_id,
			IParamValSP									ret) override;


private:
	IInterfaceType					*m_type;
	std::string						m_inst_name;
	invoke_req_f					m_req_f;

};

} /* namespace tblink_rpc_core */

