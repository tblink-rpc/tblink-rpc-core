/*
 * EndpointPythonLoopback.h
 *
 *  Created on: Mar 26, 2022
 *      Author: mballance
 */

#pragma once
#include "EndpointLoopback.h"

namespace tblink_rpc_core {

class EndpointPythonLoopback : public EndpointLoopback {
public:
	EndpointPythonLoopback();
	virtual ~EndpointPythonLoopback();
};

} /* namespace tblink_rpc_core */

