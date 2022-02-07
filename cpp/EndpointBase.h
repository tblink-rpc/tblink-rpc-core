/*
 * EndpointBase.h
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IInterfaceInst.h"
#include "tblink_rpc/IInterfaceType.h"
#include "InterfaceInstBase.h"
#include "InterfaceType.h"

namespace tblink_rpc_core {

class EndpointBase : public IEndpoint {
public:
	EndpointBase();

	virtual ~EndpointBase();

	virtual IEndpointFlags getFlags() override;

	virtual void setFlag(IEndpointFlags f) override;

	virtual int32_t init(IEndpointServices *services) override;

	virtual int32_t is_init() override;

	virtual int32_t build_complete() override;

	virtual int32_t is_build_complete() override;

	virtual int32_t connect_complete() override;

	virtual int32_t is_connect_complete() override;

	virtual IEndpoint::comm_state_e comm_state() override;

	virtual void update_comm_mode(comm_mode_e m, comm_state_e s) override;

	virtual IEndpointListener *addListener(const endpoint_ev_f &) override;

	virtual void addListener(IEndpointListener *l) override;

	virtual void removeListener(IEndpointListener *l) override;

	void sendEvent(IEndpointEvent::kind_t kind);

	void sendEvent(const IEndpointEvent *ev);

	virtual std::vector<std::string> args() override;

	virtual uint64_t time() override;

	virtual int32_t time_precision() override;

	virtual const std::string &last_error() override {
		return m_last_error;
	}

	virtual IInterfaceType *findInterfaceType(
			const std::string		&name) override;

	virtual IInterfaceTypeBuilder *newInterfaceTypeBuilder(
			const std::string		&name) override;

	virtual IInterfaceType *defineInterfaceType(
			IInterfaceTypeBuilder	*type,
			IInterfaceImplFactory	*impl_factory,
			IInterfaceImplFactory	*impl_mirror_factory) override;

	/* Implementation must provide
	virtual IInterfaceInst *defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			bool					is_mirror,
			IInterfaceImpl			*impl) override;
	 */

	/**
	 * Returns the available interface types registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
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

	virtual IParamValBool *mkValBool(bool val) override;

	virtual IParamValInt *mkValIntU(uint64_t val, int32_t width) override;

	virtual IParamValInt *mkValIntS(int64_t val, int32_t width) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVec *mkValVec() override;

protected:
	using iftype_m_t=std::unordered_map<std::string,IInterfaceTypeUP>;
	using iftype_l_t=std::vector<IInterfaceType *>;
	using ifinst_m_t=std::unordered_map<std::string,IInterfaceInstUP>;
	using ifinst_l_t=std::vector<IInterfaceInst *>;
	using ifobj_m_t=std::unordered_map<intptr_t, IInterfaceInstUP>;
	using id2ifinst_m_t=std::unordered_map<intptr_t, IInterfaceInst *>;

protected:

	int peer_init(
			int32_t								time_precision,
			const std::vector<std::string>		&args);

	int peer_build_complete();

	int32_t build_complete_check();

	int peer_connect_complete();

	int32_t connect_complete_check();

	virtual void last_error(const char *fmt, ...);

protected:
	IEndpointFlags										m_flags;
	IEndpointServicesUP									m_services;
	IEndpoint::comm_state_e								m_comm_state;
	IEndpoint::comm_mode_e								m_comm_mode;

	int32_t												m_init;
	int32_t												m_peer_init;
	int32_t												m_build_complete;
	int32_t												m_peer_build_complete;
	int32_t												m_build_check_complete;
	int32_t												m_connect_complete;
	int32_t												m_peer_connect_complete;
	int32_t												m_connect_check_complete;

	// Cached data for endpoints with remote services
	uint64_t											m_time;
	std::vector<std::string>							m_args;
	int32_t												m_time_precision;

	std::vector<IEndpointListenerUP>					m_listeners;
	std::vector<IEndpointListener *>					m_listeners_p;

	std::string											m_last_error;

	intptr_t											m_ifinst_id;
	iftype_m_t											m_peer_ifc_types;
	std::vector<IInterfaceType*>						m_peer_ifc_types_pl;
	iftype_m_t											m_local_ifc_types;
	std::vector<IInterfaceType*>						m_local_ifc_type_pl;

	ifinst_m_t											m_peer_ifc_insts;
	std::vector<IInterfaceInst*>						m_peer_ifc_insts_pl;
	ifinst_m_t											m_local_ifc_insts;
	std::vector<IInterfaceInst*>						m_local_ifc_insts_pl;

	id2ifinst_m_t										m_id2ifinst_m;

	ifobj_m_t											m_obj_m;
};

} /* namespace tblink_rpc_core */

