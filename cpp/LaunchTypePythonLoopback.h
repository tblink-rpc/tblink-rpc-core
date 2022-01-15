/*
 * LaunchTypePythonLoopback.h
 *
 *  Created on: Dec 29, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/ILaunchType.h"

namespace tblink_rpc_core {

class LaunchTypePythonLoopback : public ILaunchType {
public:
	LaunchTypePythonLoopback();

	virtual ~LaunchTypePythonLoopback();

	virtual std::string name() override { return "python.loopback"; }

	/**
	 * Launches a peer endpoint and returns the appropriate
	 * endpoint. In the case of an error, the string will
	 * contain an error message
	 *
	 * If the launcher wishes to specify the endpoint services,
	 * then that must be done now. Otherwise, the environment
	 * implementing the launcher will specify the default
	 * services for that environment
	 */
	virtual result_t launch(
			ILaunchParams 		*params,
			IEndpointServices	*services) override;

	/**
	 *
	 */
	virtual ILaunchParams *newLaunchParams() override;

private:

	int32_t get_python_info(
			const std::string		&python_path,
			std::string				&python_dll,
			std::string				&pythonpath);

};

} /* namespace tblink_rpc_core */

