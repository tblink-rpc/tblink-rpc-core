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

#include "InterfaceInstMsgTransport.h"
#include "InterfaceType.h"
#include "tblink_rpc/IParamVal.h"
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IEndpointMsg.h"
#include "tblink_rpc/IEndpointListener.h"
#include "tblink_rpc/IEndpointServices.h"


namespace tblink_rpc_core {

class EndpointMsgBase : public virtual IEndpoint, public virtual IEndpointMsg {
public:
	using response_f = std::function<void (intptr_t, IParamValMap *, IParamValMap *)>;
public:

	friend class InterfaceInstBase;

	EndpointMsgBase();

	virtual ~EndpointMsgBase();

	virtual int32_t init(
			IEndpointServices		*ep_services,
			IEndpointListener		*ep_listener) override;

	virtual int32_t is_init() override;

	virtual int32_t build_complete() override;

	virtual int32_t is_build_complete() override;

	virtual int32_t connect_complete() override;

	virtual int32_t is_connect_complete() override;

	virtual int32_t await_run_until_event() override;

	virtual bool shutdown() override;

	virtual IEndpoint::comm_state_e comm_state() override;

	virtual void update_comm_mode(comm_mode_e m, comm_state_e s) override;

	virtual IEndpointListener *addListener(const endpoint_ev_f &) override;

	virtual void addListener(IEndpointListener *l) override;

	virtual void removeListener(IEndpointListener *l) override;

	void sendEvent(IEndpointEvent::kind_t kind);

	void sendEvent(const IEndpointEvent *ev);

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


	virtual const std::string &last_error() override {
		return m_last_error;
	}

	virtual void last_error(const char *fmt, ...);

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


	virtual const std::vector<IInterfaceType *> &getInterfaceTypes() override;

	/**
	 * Returns the available interface types registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceType *> &getPeerInterfaceTypes() override;

	/**
	 * Returns the available interface instances registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceInst *> &getInterfaceInsts() override;

	/**
	 * Returns the available interface instances registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceInst *> &getPeerInterfaceInsts() override;

	virtual intptr_t send_req(
			const std::string 	&method,
			IParamValMap 		*params);

	virtual intptr_t send_req(
			const std::string 				&method,
			IParamValMap 					*params,
			const response_f				&rsp_f);

	virtual IParamValBool *mkValBool(bool val) override;

	virtual IParamValInt *mkValIntU(uint64_t val, int32_t width) override;

	virtual IParamValInt *mkValIntS(int64_t val, int32_t width) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVec *mkValVec() override;

	int32_t recv_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMap			*params);

	int32_t recv_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error);

	virtual int32_t send_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMap			*params) = 0;

	virtual int32_t send_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error) = 0;

	std::pair<IParamValMapUP,IParamValMapUP> wait_rsp(intptr_t id);

private:
	// Methods used by InterfaceInst

	IParamVal *invoke_nb(
			InterfaceInstBase	*ifinst,
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

	rsp_t req_set_comm_state(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_set_comm_mode(
			intptr_t		id,
			IParamValMap 	*params);

	void call_completion_nb(
			intptr_t		id,
			intptr_t		call_id,
			IParamVal		*retval);

	void call_completion_b(
			InterfaceInstBase	*ifinst,
			intptr_t			call_id,
			IParamVal			*retval);

	void rsp_call_completion_b(
			intptr_t			rsp_id,
			IParamValMap		*result,
			IParamValMap		*error);

	IParamValMap *pack_iftypes(
			const std::unordered_map<std::string,InterfaceTypeUP>	&iftypes);

	IParamValMap *pack_ifinsts(
			const std::unordered_map<std::string,InterfaceInstMsgTransportUP>	&ifinsts);

	IParamValMap *pack_type(IType *t);

	void unpack_iftypes(
			std::unordered_map<std::string,InterfaceTypeUP>		&iftypes,
			std::vector<IInterfaceType *>						&iftypes_l,
			IParamValMap 										*iftypes_p);

	void unpack_ifinsts(
			std::unordered_map<std::string,InterfaceInstMsgTransportUP>	&ifinsts,
			std::vector<IInterfaceInst*>								&ifinsts_l,
			IParamValMap  												*ifinsts_p);

private:
	typedef std::pair<bool, std::pair<IParamValMap*,IParamValMap*>> rspq_elem_t;

protected:
	IEndpointServicesUP												m_services;

private:
	intptr_t														m_id;
	std::string														m_last_error;

	std::unordered_map<intptr_t, response_f>						m_rsp_m;

	int32_t															m_init;
	int32_t															m_peer_init;
	int32_t															m_build_complete;
	int32_t															m_peer_build_complete;
	int32_t															m_connect_complete;
	int32_t															m_peer_connect_complete;
	int32_t															m_peer_local_check_complete;
	std::map<intptr_t, std::function<void()>>						m_callback_m;
	std::set<intptr_t>												m_pending_time_cb;
	intptr_t														m_callback_id;
	std::map<std::string,req_func_t>								m_req_m;

	std::map<std::string, IInterfaceTypeUP>							m_iftype_m;
	uint64_t														m_time;

	std::vector<std::string>										m_args;
	int32_t															m_time_precision;

	std::unordered_map<std::string, InterfaceTypeUP>				m_peer_ifc_types;
	std::vector<IInterfaceType*>									m_peer_ifc_types_pl;
	std::unordered_map<std::string, InterfaceTypeUP>				m_local_ifc_types;
	std::vector<IInterfaceType*>									m_local_ifc_type_pl;

	std::unordered_map<std::string, InterfaceInstMsgTransportUP>	m_peer_ifc_insts;
	std::vector<IInterfaceInst*>									m_peer_ifc_insts_pl;
	std::unordered_map<std::string, InterfaceInstMsgTransportUP>	m_local_ifc_insts;
	std::vector<IInterfaceInst*>									m_local_ifc_insts_pl;

	int32_t															m_run_until_event;
	int32_t															m_event_received;
	std::vector<IEndpointListenerUP>								m_listeners;
	std::vector<IEndpointListener *>								m_listeners_p;
	IEndpoint::comm_state_e											m_comm_state;
	IEndpoint::comm_mode_e											m_comm_mode;
	uint32_t														m_pending_blocking_calls;

	int32_t															m_release_reqs;
	int32_t															m_wait_reqs;

};

} /* namespace tblink_rpc_core */

