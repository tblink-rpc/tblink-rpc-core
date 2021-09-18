/*
 * Endpoint.h
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#pragma once
#include <map>
#include <unordered_map>
#include <stdint.h>

#include "InterfaceInst.h"
#include "InterfaceType.h"
#include "tblink_rpc/IParamVal.h"
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IEndpointListener.h"
#include "tblink_rpc/IEndpointServices.h"
#include "tblink_rpc/ITransport.h"


namespace tblink_rpc_core {

class EndpointMsgTransport : public virtual IEndpoint {
public:
	using response_f = std::function<void (intptr_t, IParamValMap *, IParamValMap *)>;
public:

	friend class InterfaceInst;

	EndpointMsgTransport(ITransport	*transport);

	virtual ~EndpointMsgTransport();

	virtual int32_t init(
			IEndpointServices		*ep_services,
			IEndpointListener		*ep_listener) override;

	virtual State state() override { return m_state; }

	virtual int32_t is_init() override;

	virtual int32_t build_complete() override;

	virtual int32_t is_build_complete() override;

	virtual int32_t connect_complete() override;

	virtual int32_t is_connect_complete() override;

	virtual bool shutdown() override;

	virtual int32_t yield() override;

	virtual int32_t run_until_event() override;

	virtual int32_t await_req() override;

	virtual int32_t yield_blocking() override;

	virtual intptr_t add_time_callback(
			uint64_t						time,
			const std::function<void()>		&cb_f) override;

	virtual void cancel_callback(intptr_t	id) override;

	virtual const std::vector<std::string> &args() override;

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
			bool					is_mirror,
			const invoke_req_f		&req_f) override;

	/**
	 * Poll, waiting for a message to be received.
	 * Returns -1 on transport error.
	 */
	virtual int32_t process_one_message() override;

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

	virtual IParamValInt *mkValIntU(uint64_t val, int32_t width) override;

	virtual IParamValInt *mkValIntS(int64_t val, int32_t width) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVec *mkValVec() override;

	intptr_t send_req(
			const std::string 	&method,
			IParamValMap 		*params);

	intptr_t send_req(
			const std::string 				&method,
			IParamValMap 					*params,
			const response_f				&rsp_f);

	std::pair<IParamValMapUP,IParamValMapUP> wait_rsp(intptr_t id);

private:
	// Methods used by InterfaceInst

	IParamVal *invoke_nb(
			InterfaceInst	*ifinst,
			IMethodType 		*method,
			IParamValVec 	*params);

private:
	typedef std::pair<IParamValMapUP,IParamValMapUP> rsp_t;
	typedef std::function<rsp_t(intptr_t, IParamValMap *)> req_func_t;
private:

	rsp_t req_init(
			intptr_t		id,
			IParamValMap 	*params);

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
			InterfaceInst	*ifinst,
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


	IParamValMap *pack_iftypes(
			const std::unordered_map<std::string,InterfaceTypeUP>	&iftypes);

	IParamValMap *pack_ifinsts(
			const std::unordered_map<std::string,InterfaceInstUP>	&ifinsts);

	IParamValMap *pack_type(IType *t);

	void unpack_iftypes(
			std::unordered_map<std::string,InterfaceTypeUP>		&iftypes,
			IParamValMap 										*iftypes_p);

	void unpack_ifinsts(
			std::unordered_map<std::string,InterfaceInstUP>		&ifinsts,
			IParamValMap  										*ifinsts_p);

private:
	typedef std::pair<bool, std::pair<IParamValMap*,IParamValMap*>> rspq_elem_t;


private:
	ITransport											*m_transport;
	IEndpointServices									*m_services;
	intptr_t											m_id;

	std::unordered_map<intptr_t, response_f>			m_rsp_m;

	int32_t												m_init;
	int32_t												m_peer_init;
	int32_t												m_build_complete;
	int32_t												m_peer_build_complete;
	int32_t												m_connect_complete;
	int32_t												m_peer_connect_complete;
	int32_t												m_peer_local_check_complete;
	std::map<intptr_t, std::function<void()>>			m_callback_m;
	std::set<intptr_t>									m_pending_time_cb;
	intptr_t											m_callback_id;
	std::map<std::string,req_func_t>					m_req_m;

	State															m_state;

	std::map<std::string, IInterfaceTypeUP>							m_iftype_m;
	uint64_t														m_time;

	std::vector<std::string>										m_args;
	int32_t															m_time_precision;

	std::unordered_map<std::string, InterfaceTypeUP>				m_peer_ifc_types;
	std::unordered_map<std::string, InterfaceTypeUP>				m_local_ifc_types;
	std::vector<IInterfaceType*>									m_local_ifc_type_pl;

	std::unordered_map<std::string, InterfaceInstUP>				m_peer_ifc_insts;
	std::unordered_map<std::string, InterfaceInstUP>				m_local_ifc_insts;
	std::vector<IInterfaceInst*>									m_local_ifc_insts_pl;

	int32_t															m_event_received;

};

} /* namespace tblink_rpc_core */

