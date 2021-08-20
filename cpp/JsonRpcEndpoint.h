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

	friend class JsonInterfaceInst;

	JsonRpcEndpoint(
			IEndpoint::Type		type,
			IEndpointServices	*services);

	virtual ~JsonRpcEndpoint();

	void init(ITransport *transport);

	virtual State state() override { return m_state; }

	virtual Type type() override { return m_type; }

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

	virtual int32_t time_precision() override {
		return m_time_precision;
	}

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

	virtual IParamValBool *mkValBool(bool val) override;

	virtual IParamValInt *mkValIntU(uint64_t val) override;

	virtual IParamValInt *mkValIntS(int64_t val) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVector *mkVector() override;

	intptr_t send_req(
			const std::string 	&method,
			IParamValMap 		*params,
			bool				active_wait=true);

	std::pair<IParamValMapUP,IParamValMapUP> wait_rsp(intptr_t id);

private:
	// Methods used by InterfaceInst

	IParamVal *invoke_nb(
			JsonInterfaceInst	*ifinst,
			IMethodType 		*method,
			IParamValVector 	*params);

private:
	typedef std::pair<IParamValMapUP,IParamValMapUP> rsp_t;
	typedef std::function<rsp_t(intptr_t, IParamValMap *)> req_func_t;
private:
	rsp_t req_build_complete(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_connect_complete(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_get_interface_types(
			intptr_t		id,
			IParamValMap	*params);

	rsp_t req_add_time_callback(
			intptr_t		id,
			IParamValMap	*params);

	rsp_t req_notify_callback(
			intptr_t		id,
			IParamValMap	*params);

	rsp_t req_shutdown(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_invoke_nb(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_invoke_b(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_invoke_rsp_b(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_run_until_event(
			intptr_t		id,
			IParamValMap 	*params);

	void call_completion_nb(
			intptr_t		id,
			IParamVal		*retval);

	void call_completion_b(
			JsonInterfaceInst	*ifinst,
			intptr_t			call_id,
			IParamVal			*retval);

	int32_t recv_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMap			*params);

	int32_t recv_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error);


	IParamValMap *pack_iftypes();

	IParamValMap *pack_ifinsts();

	std::vector<JsonInterfaceTypeUP> unpack_iftypes(IParamValMap *iftypes);

	std::vector<JsonInterfaceInstUP> unpack_ifinsts(IParamValMap  *ifinsts);

private:
	typedef std::pair<bool, std::pair<IParamValMap*,IParamValMap*>> rspq_elem_t;


private:
	ITransport						*m_transport;
	IEndpoint::Type					m_type;
	IEndpointServices				*m_services;

	std::map<intptr_t, rspq_elem_t>									m_rsp_m;

	bool															m_build_complete;
	bool															m_connect_complete;
	std::map<intptr_t, std::function<void()>>						m_callback_m;
	std::set<intptr_t>												m_pending_time_cb;
	intptr_t														m_callback_id;
	std::map<std::string,req_func_t>								m_req_m;

	State															m_state;

	std::map<std::string, IInterfaceTypeUP>							m_iftype_m;
	uint64_t														m_time;
	int32_t															m_time_precision;

	std::map<std::string, JsonInterfaceType*>						m_local_ifc_types;
	std::vector<JsonInterfaceTypeUP>								m_local_ifc_type_l;
	std::vector<IInterfaceType*>									m_local_ifc_type_pl;
	std::map<std::string, JsonInterfaceInst*>						m_local_ifc_insts;
	std::vector<JsonInterfaceInstUP>								m_local_ifc_insts_l;
	std::vector<IInterfaceInst*>									m_local_ifc_insts_pl;

	int32_t															m_event_received;

};

} /* namespace tblink_rpc_core */

