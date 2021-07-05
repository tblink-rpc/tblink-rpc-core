/*
 * Endpoint.h
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#pragma once

#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/ITransport.h"

namespace tblink_rpc_core {

class JsonRpcEndpoint : public virtual IEndpoint {
public:
	JsonRpcEndpoint(
			ITransport		*transport);

	virtual ~JsonRpcEndpoint();

	virtual bool build_complete() override;

	virtual bool connect_complete() override;

	virtual const std::string &last_error() override { }


	virtual IInterfaceTypeBuilder *newInterfaceTypeBuilder(
			const std::string		&name) override { }

	virtual IInterfaceType *defineInterfaceType(
			IInterfaceTypeBuilder	*type) override { }

	virtual IInterfaceInst *defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			const invoke_req_f		&req_f) override { }

private:
	int32_t recv_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMapSP			params);

	int32_t recv_rsp(
			intptr_t				id,
			IParamValSP				result,
			IParamValSP				error);

private:
	ITransport						*m_transport;

};

} /* namespace tblink_rpc_core */

