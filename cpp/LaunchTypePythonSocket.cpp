/*
 * LaunchTypePythonSocket.cpp
 *
 *  Created on: Dec 26, 2021
 *      Author: mballance
 */

#include "LaunchTypePythonSocket.h"
#include "LaunchTypeRegistration.h"

namespace tblink_rpc_core {

LaunchTypePythonSocket::LaunchTypePythonSocket() {
	// TODO Auto-generated constructor stub

}

LaunchTypePythonSocket::~LaunchTypePythonSocket() {
	// TODO Auto-generated destructor stub
}

ILaunchType::result_t LaunchTypePythonSocket::launch(
			ILaunchParams 			*params,
			IEndpointServices		*services) {
	ILaunchType::result_t ret = {0, ""};

	// TOOD: check first

	if (!params->has_param("module")) {
		ret.second = "No 'module' parameter specified";
		return ret;
	}

	std::string python_path = "python3";
	if (params->has_param("python")	) {
		python_path = params->get_param("python");
	}  // TODO: honor environment variable

	ILaunchParams *proc_params = LaunchTypeProcessSocket::newLaunchParams();
	proc_params->add_arg(python_path);
	proc_params->add_arg("-m");
	proc_params->add_arg(params->get_param("module"));

	for (auto pval_it=params->params().begin();
			pval_it!=params->params().end(); pval_it++) {
		proc_params->add_param(pval_it->first, pval_it->second);
	}

	delete params;

	ret = LaunchTypeProcessSocket::launch(proc_params, services);

	return ret;
}

static LaunchTypeRegistration<LaunchTypePythonSocket>	m_registration;

} /* namespace tblink_rpc_core */
