/*
 * Endpoint.h
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#pragma once
#include <map>
#include <stdint.h>
#include "JsonInterfaceType.h"
#include "JsonInterfaceInst.h"
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

	virtual int32_t run_until_event() override;

	virtual int32_t await_req() override;

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

	/**
	 * Returns the available interface types registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceType *> &getInterfaceTypes() override;

	/**
	 * Returns the available interface instances registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceInst *> &getInterfaceInsts() override;

	virtual IParamValBoolSP mkValBool(bool val) override;

	virtual IParamValIntSP mkValIntU(uint64_t val) override;

	virtual IParamValIntSP mkValIntS(int64_t val) override;

	virtual IParamValMapSP mkValMap() override;

	virtual IParamValStrSP mkValStr(const std::string &val) override;

	virtual IParamValVectorSP mkVector() override;

	intptr_t send_req(const std::string &method, IParamValMapSP params);

	std::pair<IParamValMapSP,IParamValMapSP> wait_rsp(intptr_t id);

private:
	typedef std::pair<IParamValMapSP,IParamValMapSP> rsp_t;
	typedef std::function<rsp_t(intptr_t, IParamValMapSP)> req_func_t;
private:
	rsp_t req_build_complete(
			intptr_t		id,
			IParamValMapSP 	params);

	rsp_t req_connect_complete(
			intptr_t		id,
			IParamValMapSP 	params);

	rsp_t req_get_interface_types(
			intptr_t		id,
			IParamValMapSP 	params);

	rsp_t req_add_time_callback(
			intptr_t		id,
			IParamValMapSP 	params);

	rsp_t req_notify_callback(
			intptr_t		id,
			IParamValMapSP 	params);

	rsp_t req_shutdown(
			intptr_t		id,
			IParamValMapSP 	params);

	rsp_t req_invoke_nb(
			intptr_t		id,
			IParamValMapSP 	params);

	rsp_t req_run_until_event(
			intptr_t		id,
			IParamValMapSP 	params);

	void call_completion(
			intptr_t		id,
			IParamValSP		retval);

	int32_t recv_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMapSP			params);

	int32_t recv_rsp(
			intptr_t				id,
			IParamValMapSP			result,
			IParamValMapSP			error);


	std::pair<IParamValMapSP,IParamValMapSP> get_interface_types(
			);


	IParamValMapSP pack_iftypes();

	IParamValMapSP pack_ifinsts();

	std::vector<JsonInterfaceTypeUP> unpack_iftypes(IParamValMapSP iftypes);

	std::vector<JsonInterfaceInstUP> unpack_ifinsts(IParamValMapSP ifinsts);


private:
	ITransport						*m_transport;
	IEndpointServices				*m_services;

	std::map<intptr_t, std::pair<IParamValMapSP,IParamValMapSP>>	m_rsp_m;

	bool															m_build_complete;
	bool															m_connect_complete;
	std::map<intptr_t, std::function<void()>>						m_callback_m;
	std::set<intptr_t>												m_pending_time_cb;
	intptr_t														m_callback_id;
	std::map<std::string,req_func_t>								m_req_m;

	State															m_state;

	std::map<std::string, IInterfaceTypeUP>							m_iftype_m;
	uint64_t														m_time;

	std::map<std::string, JsonInterfaceType*>						m_local_ifc_types;
	std::vector<JsonInterfaceTypeUP>								m_local_ifc_type_l;
	std::vector<IInterfaceType*>									m_local_ifc_type_pl;
	std::map<std::string, JsonInterfaceInst*>						m_local_ifc_insts;
	std::vector<JsonInterfaceInstUP>								m_local_ifc_insts_l;
	std::vector<IInterfaceInst*>									m_local_ifc_insts_pl;

	int32_t															m_event_received;

	// Set 'true' when the endpoint is in 'await-event' mode
	bool															m_await_event;

};

} /* namespace tblink_rpc_core */

