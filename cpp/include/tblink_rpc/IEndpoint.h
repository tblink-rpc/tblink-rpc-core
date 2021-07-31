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
#include "tblink_rpc/IInterfaceInst.h"
#include "tblink_rpc/IInterfaceType.h"
#include "tblink_rpc/IInterfaceTypeBuilder.h"

namespace tblink_rpc_core {

class IEndpoint;
typedef std::unique_ptr<IEndpoint> IEndpointUP;
class IEndpoint : public virtual IParamValFactory {
public:
	enum State {
		Init,
		Built,
		Connected,
		Running,
		Shutdown
	};

public:

	virtual ~IEndpoint() { }

	virtual State state() = 0;

	virtual bool build_complete() = 0;

	virtual bool connect_complete() = 0;

	virtual bool shutdown() = 0;

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

	virtual uint64_t time() = 0;

	virtual int32_t time_precision() = 0;

	/** Called by the environment to notify that
	 *  a callback has occurred
	 */
	virtual void notify_callback(intptr_t   id) = 0;

	virtual const std::string &last_error() = 0;

	virtual IInterfaceType *findInterfaceType(
			const std::string		&name) = 0;

	/*
	virtual ITypeBuilderSP mk_type_builder(
			const std::string		&name) = 0;

	virtual ITypeSP define_interface_type(
			ITypeBuilderSP			type) = 0;
			 */

	virtual IInterfaceTypeBuilder *newInterfaceTypeBuilder(
			const std::string		&name) = 0;

	virtual IInterfaceType *defineInterfaceType(
			IInterfaceTypeBuilder	*type) = 0;

	virtual IInterfaceInst *defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			const invoke_req_f		&req_f) = 0;

	/**
	 * Returns the available interface types registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceType *> &getInterfaceTypes() = 0;

	/**
	 * Returns the available interface instances registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceInst *> &getInterfaceInsts() = 0;

//	virtual IInterfaceInst *findInterfaceInst(const std::string &name) = 0;

	// TODO: register type
	//       - could have an object-lifetime manager attached

	// TODO: register static instance

};

}

