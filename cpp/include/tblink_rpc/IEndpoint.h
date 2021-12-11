/*
 * IEndpoint.h
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */

#pragma once
#include <functional>
#include <string>
//#include "IType.h"
//#include "ITypeBuilder.h"
#include "tblink_rpc/IEndpointListener.h"
#include "tblink_rpc/IEndpointServices.h"
#include "tblink_rpc/IInterfaceInst.h"
#include "tblink_rpc/IInterfaceType.h"
#include "tblink_rpc/IInterfaceTypeBuilder.h"

namespace tblink_rpc_core {

using time_cb_f=std::function<void()>;

class IEndpoint;
typedef std::unique_ptr<IEndpoint> IEndpointUP;
class IEndpoint : public virtual IParamValFactory {
public:

	enum comm_state_e {
		Waiting,
		Released
	};

	virtual ~IEndpoint() { }

	virtual int32_t init(
			IEndpointServices		*ep_services,
			IEndpointListener		*ep_listener) = 0;

	virtual int32_t is_init() = 0;

	virtual int32_t build_complete() = 0;

	virtual int32_t is_build_complete() = 0;

	virtual int32_t connect_complete() = 0;

	virtual int32_t is_connect_complete() = 0;

	virtual IEndpointListener *addListener(const endpoint_ev_f &) = 0;

	virtual void addListener(IEndpointListener *l) = 0;

	virtual void removeListener(IEndpointListener *l) = 0;

	/**
	 * Process messages until a run-until-event
	 * request is received
	 */
	virtual int32_t await_run_until_event() = 0;

	virtual bool shutdown() = 0;

	virtual comm_state_e comm_state() = 0;

	/**
	 * Yield control to enable message processing.
	 * Returns 'true' if activity was processed and
	 * false when idle.
	 */
	virtual int32_t yield() = 0;

	virtual int32_t run_until_event() = 0;

	virtual int32_t await_req() = 0;

	/**
	 * Process messages until the environment has blocking work to do
	 */
	virtual int32_t yield_blocking() = 0;

	virtual intptr_t add_time_callback(
			uint64_t						time,
			const std::function<void()>		&cb_f) = 0;

	virtual void cancel_callback(intptr_t	id) = 0;

	virtual const std::vector<std::string> &args() = 0;

	virtual uint64_t time() = 0;

	virtual int32_t time_precision() = 0;

	/** Called by the environment to notify that
	 *  a callback has occurred
	 */
	virtual void notify_callback(intptr_t   id) = 0;

	virtual const std::string &last_error() = 0;

	virtual IInterfaceType *findInterfaceType(
			const std::string		&name) = 0;

	virtual IInterfaceTypeBuilder *newInterfaceTypeBuilder(
			const std::string		&name) = 0;

	virtual IInterfaceType *defineInterfaceType(
			IInterfaceTypeBuilder	*type) = 0;

	virtual IInterfaceInst *defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			bool					is_mirror,
			const invoke_req_f		&req_f) = 0;

	/**
	 * Poll, waiting for a message to be received.
	 * Returns -1 on transport error.
	 */
	virtual int32_t process_one_message() = 0;

	virtual const std::vector<IInterfaceType *> &getInterfaceTypes() = 0;

	/**
	 * Returns the available interface types registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceType *> &getPeerInterfaceTypes() = 0;

	virtual const std::vector<IInterfaceInst *> &getInterfaceInsts() = 0;

	/**
	 * Returns the available interface instances registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceInst *> &getPeerInterfaceInsts() = 0;

//	virtual IInterfaceInst *findInterfaceInst(const std::string &name) = 0;

	// TODO: register type
	//       - could have an object-lifetime manager attached

	// TODO: register static instance

};

}

