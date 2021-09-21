/*
 * LaunchTypeProcessSocket.h
 *
 *  Created on: Sep 19, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/ILaunchType.h"

namespace tblink_rpc_core {

/**
 * Launches the peer endpoint as a process that connects
 * back via a TCP/IP socket connection. The launched
 * process has TBLINK_HOST set to the hostname of the
 * machine running the server ; The launched process
 * has TBLINK_PORT set to the port number of the server.
 */
class LaunchTypeProcessSocket : public ILaunchType {
public:
	LaunchTypeProcessSocket();

	virtual ~LaunchTypeProcessSocket();

	virtual std::string name() { return "process.socket"; }

	/**
	 * Launches a peer endpoint and returns the appropriate
	 * endpoint. In the case of an error, the string will
	 * contain an error message
	 */
	virtual result_t launch(ILaunchParams *params) override;

};

} /* namespace tblink_rpc_core */

