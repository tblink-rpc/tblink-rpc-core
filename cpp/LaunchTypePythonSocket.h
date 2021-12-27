/*
 * LaunchTypePythonSocket.h
 *
 *  Created on: Dec 26, 2021
 *      Author: mballance
 */

#pragma once
#include "LaunchTypeProcessSocket.h"

namespace tblink_rpc_core {

class LaunchTypePythonSocket : public LaunchTypeProcessSocket {
public:
	LaunchTypePythonSocket();

	virtual ~LaunchTypePythonSocket();

	virtual std::string name() override { return "python.socket"; }

	/**
	 * Launches a peer endpoint and returns the appropriate
	 * endpoint. In the case of an error, the string will
	 * contain an error message
	 */
	virtual result_t launch(
			ILaunchParams 			*params,
			IEndpointServices		*services) override;

};

} /* namespace tblink_rpc_core */

