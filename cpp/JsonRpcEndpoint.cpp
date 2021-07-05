/*
 * Endpoint.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#include "JsonRpcEndpoint.h"

namespace tblink_rpc_core {

JsonRpcEndpoint::JsonRpcEndpoint(ITransport *t) {
	m_transport = t;

	m_transport->init(
			std::bind(
					&JsonRpcEndpoint::recv_req,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3),
			std::bind(
					&JsonRpcEndpoint::recv_rsp,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3)
					);

}

JsonRpcEndpoint::~JsonRpcEndpoint() {
	// TODO Auto-generated destructor stub
}

bool JsonRpcEndpoint::build_complete() {

}

bool JsonRpcEndpoint::connect_complete() {

}

int32_t JsonRpcEndpoint::recv_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMapSP			params) {
	;
}

int32_t JsonRpcEndpoint::recv_rsp(
		intptr_t				id,
		IParamValSP				result,
		IParamValSP				error) {
	;
}

} /* namespace tblink_rpc_core */
