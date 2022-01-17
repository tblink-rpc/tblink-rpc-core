/*
 * InterfaceInstMsg.cpp
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

#include "Debug.h"
#include "EndpointMsgTransport.h"
#include "InterfaceInstMsgTransport.h"

#define DEBUG_INTERFACE_INST_MSG_TRANSPORT

#ifdef DEBUG_INTERFACE_INST_MSG_TRANSPORT
#define DEBUG_ENTER(fmt, ...) \
	DEBUG_ENTER_BASE(InterfaceInstMsgTransport, fmt, ##__VA_ARGS__)
#define DEBUG_LEAVE(fmt, ...) \
	DEBUG_LEAVE_BASE(InterfaceInstMsgTransport, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) \
	DEBUG_BASE(InterfaceInstMsgTransport, fmt, ##__VA_ARGS__)
#else
#define DEBUG_ENTER(fmt, ...)
#define DEBUG_LEAVE(fmt, ...)
#define DEBUG(fmt, ...)
#endif

namespace tblink_rpc_core {

InterfaceInstMsgTransport::InterfaceInstMsgTransport(
		EndpointMsgBase				*endpoint,
		IInterfaceType				*type,
		const std::string			&inst_name,
		bool						is_mirror) : InterfaceInstBase(
				endpoint, type, inst_name, is_mirror) {
}

InterfaceInstMsgTransport::InterfaceInstMsgTransport(
		EndpointMsgBase				*endpoint,
		IInterfaceType				*type,
		const std::string			&inst_name,
		bool						is_mirror,
		IInterfaceImpl				*impl) : InterfaceInstBase(
				endpoint, type, inst_name, is_mirror, impl) {

}

InterfaceInstMsgTransport::~InterfaceInstMsgTransport() {
	// TODO Auto-generated destructor stub
}

int32_t InterfaceInstMsgTransport::invoke(
		IMethodType									*method,
		IParamValVec								*params,
		const invoke_rsp_f							&completion_f) {
	int32_t ret = 0;
	DEBUG_ENTER("invoke");
	IParamValMap *r_params = m_endpoint->mkValMap();
	intptr_t call_id = m_call_id;
	m_call_id += 1;

	ret = dynamic_cast<EndpointMsgBase *>(m_endpoint)->invoke(
			this,
			method,
			params,
			completion_f);

	DEBUG_LEAVE("invoke");
	return ret;
}

} /* namespace tblink_rpc_core */
