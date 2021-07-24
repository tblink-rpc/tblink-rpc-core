/*
 * Endpoint.h
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#pragma once
#include <map>
#include <stdint.h>
#include "tblink_rpc/IParamVal.h"
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IEndpointServices.h"
#include "tblink_rpc/ITransport.h"

namespace tblink_rpc_core {

class JsonRpcEndpoint : public virtual IEndpoint {
public:
	JsonRpcEndpoint(
			ITransport			*transport,
			IEndpointServices	*services);

	virtual ~JsonRpcEndpoint();

	virtual State state() override { return m_state; }

	virtual bool build_complete() override;

	virtual bool connect_complete() override;

	virtual bool shutdown() override;

	virtual int32_t yield() override;

	virtual int32_t yield_blocking() override;

	virtual intptr_t add_time_callback(
			uint64_t						time,
			const std::function<void()>		&cb_f) override;

	virtual void cancel_callback(intptr_t	id) override;

	virtual uint64_t time() override;

	/** Called by the environment to notify that
	 *  a callback has occurred
	 */
	virtual void notify_callback(intptr_t   id) override;


	virtual const std::string &last_error() override { }

	virtual IInterfaceType *findInterfaceType(
			const std::string		&name) override;

	virtual IInterfaceTypeBuilder *newInterfaceTypeBuilder(
			const std::string		&name) override;

	virtual IInterfaceType *defineInterfaceType(
			IInterfaceTypeBuilder	*type) override;

	virtual IInterfaceInst *defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			const invoke_req_f		&req_f) override;

private:
	int32_t recv_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMapSP			params);

	int32_t recv_rsp(
			intptr_t				id,
			IParamValMapSP			result,
			IParamValMapSP			error);

	std::pair<IParamValMapSP,IParamValMapSP> wait_rsp(intptr_t id);

private:
	ITransport						*m_transport;
	IEndpointServices				*m_services;

	std::map<intptr_t, std::pair<IParamValMapSP,IParamValMapSP>>	m_rsp_m;

	bool															m_build_complete;
	bool															m_connect_complete;
	std::map<intptr_t, std::function<void()>>						m_callback_m;
	std::set<intptr_t>												m_pending_time_cb;
	intptr_t														m_callback_id;

	State															m_state;

	std::map<std::string, IInterfaceTypeUP>							m_iftype_m;
	uint64_t														m_time;

};

} /* namespace tblink_rpc_core */

