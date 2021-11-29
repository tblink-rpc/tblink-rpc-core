/*
 * EndpointLoopback.h
 *
 *  Created on: Nov 26, 2021
 *      Author: mballance
 */

#pragma once
#include "EndpointBase.h"

namespace tblink_rpc_core {

class EndpointLoopback;
using EndpointLoopbackUP=std::unique_ptr<EndpointLoopback>;

class EndpointLoopback : public EndpointBase {
public:
	EndpointLoopback();

	EndpointLoopback(EndpointLoopback *peer);

	virtual ~EndpointLoopback();

	bool primary() const { return m_primary; }

	EndpointLoopback *peer() const { return m_peer; }

	virtual State state() override { return IEndpoint::Connected; }

	virtual int32_t is_init() { return 1; }

	virtual int32_t build_complete() override;

	virtual int32_t is_build_complete() { return 1; }

	virtual int32_t connect_complete() { return 1; }

	virtual int32_t is_connect_complete() { return 1; }

	/**
	 * Process messages until a run-until-event
	 * request is received
	 */
	virtual int32_t await_run_until_event() { return 1; }

	virtual bool shutdown() { return false; }

	/**
	 * Yield control to enable message processing.
	 * Returns 'true' if activity was processed and
	 * false when idle.
	 */
	virtual int32_t yield() { return 0; }

	virtual int32_t run_until_event() { return 0; }

	virtual int32_t await_req() { return 0; }

	virtual void notify_callback(intptr_t   id) { }

	virtual int32_t process_one_message() { return 1; }

	/**
	 * Process messages until the environment has blocking work to do
	 */
	virtual int32_t yield_blocking() { return 0; }

	virtual intptr_t add_time_callback(
			uint64_t						time,
			const std::function<void()>		&cb_f) { return 0; }

	virtual void cancel_callback(intptr_t	id) { }

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
	 * Returns the available interface types registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceType *> &getInterfaceTypes() override;

	/**
	 * Returns the available interface instances registered by the
	 * endpoint peer. Only valid after 'build' is complete
	 */
	virtual const std::vector<IInterfaceInst *> &getInterfaceInsts() override;

private:
	bool							m_primary;
	EndpointLoopback				*m_peer;
	EndpointLoopbackUP				m_peer_u;

};

} /* namespace tblink_rpc_core */

