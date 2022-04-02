/*
 * EndpointPythonLoopback.h
 *
 *  Created on: Mar 26, 2022
 *      Author: mballance
 */

#pragma once
#include "EndpointLoopback.h"
#include "PythonApi.h"

namespace tblink_rpc_core {

class EndpointPythonLoopback : public EndpointLoopback {
public:
	EndpointPythonLoopback(
			const PythonApi &api,
			void			*main_t);

	virtual ~EndpointPythonLoopback();

	bool init();

	bool launch(
			const std::string				&main_module,
			const std::vector<std::string>	&args);

	virtual int32_t send_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMap			*params) override;

	virtual int32_t send_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error) override;

private:
	EndpointPythonLoopback(EndpointPythonLoopback *peer);

private:
	PythonApi					m_api;
	void						*m_interp;
	void						*m_main_t;
};

} /* namespace tblink_rpc_core */

