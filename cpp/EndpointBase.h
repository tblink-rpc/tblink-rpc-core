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

namespace tblink_rpc_core {

class EndpointBase : public IEndpoint {
public:
	EndpointBase();

	virtual ~EndpointBase();

	virtual int32_t init(
			IEndpointServices		*ep_services,
			IEndpointListener		*ep_listener) override;

	virtual IEndpoint::comm_state_e comm_state() override;

	virtual void update_comm_mode(comm_mode_e m, comm_state_e s) override;

	virtual IEndpointListener *addListener(const endpoint_ev_f &) override;

	virtual void addListener(IEndpointListener *l) override;

	virtual void removeListener(IEndpointListener *l) override;

	void sendEvent(IEndpointEvent::kind_t kind);

	void sendEvent(const IEndpointEvent *ev);

	virtual const std::vector<std::string> &args() override;

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
			IInterfaceTypeBuilder	*type) override;

	/**
	 * Returns the available interface types registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceType *> &getInterfaceTypes() override {
		return m_iftypes;
	}

	/**
	 * Returns the available interface instances registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceInst *> &getInterfaceInsts() override {
		return m_ifinsts;
	}

	virtual IParamValBool *mkValBool(bool val) override;

	virtual IParamValInt *mkValIntU(uint64_t val, int32_t width) override;

	virtual IParamValInt *mkValIntS(int64_t val, int32_t width) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVec *mkValVec() override;

protected:
	IEndpointServices									*m_services;
	IEndpointListener									*m_listener;

	std::vector<IInterfaceType *>						m_iftypes;
	std::unordered_map<std::string, IInterfaceTypeUP>	m_iftype_m;
	std::vector<IInterfaceInst *>						m_ifinsts;
	std::unordered_map<std::string, IInterfaceInstUP>	m_ifinst_m;
	std::vector<IEndpointListenerUP>					m_listeners;
	std::vector<IEndpointListener *>					m_listeners_p;

	std::string											m_last_error;
};

} /* namespace tblink_rpc_core */

