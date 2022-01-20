/*
 * EndpointMsgTransport.cpp
 *
 *  Created on: Dec 8, 2021
 *      Author: mballance
 */

#include "EndpointMsgTransport.h"

namespace tblink_rpc_core {

EndpointMsgTransport::EndpointMsgTransport(
		ITransport *transport) : m_transport(transport) {
	// We already know that this endpoint is claimed
	setFlag(IEndpointFlags::Claimed);

	m_transport->init(
			std::bind(
					&EndpointMsgTransport::recv_req,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3),
			std::bind(
					&EndpointMsgTransport::recv_rsp,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3)
					);
}

EndpointMsgTransport::~EndpointMsgTransport() {
	// TODO Auto-generated destructor stub
}

int32_t EndpointMsgTransport::process_one_message() {
	return m_transport->process_one_message();
}

int32_t EndpointMsgTransport::send_req(
		const std::string 	&method,
		intptr_t			id,
		IParamValMap		*params) {
	return m_transport->send_req(method, id, params);
}

int32_t EndpointMsgTransport::send_rsp(
		intptr_t			id,
		IParamValMap		*result,
		IParamValMap		*error) {
	return m_transport->send_rsp(id, result, error);
}

IParamValBool *EndpointMsgTransport::mkValBool(bool val) {
	return m_transport->mkValBool(val);
}

IParamValInt *EndpointMsgTransport::mkValIntU(uint64_t val, int32_t width) {
	return m_transport->mkValIntU(val, width);
}

IParamValInt *EndpointMsgTransport::mkValIntS(int64_t val, int32_t width) {
	return m_transport->mkValIntS(val, width);
}

IParamValMap *EndpointMsgTransport::mkValMap() {
	return m_transport->mkValMap();
}

IParamValStr *EndpointMsgTransport::mkValStr(const std::string &val) {
	return m_transport->mkValStr(val);
}

IParamValVec *EndpointMsgTransport::mkValVec() {
	return m_transport->mkValVec();
}

} /* namespace tblink_rpc_core */
