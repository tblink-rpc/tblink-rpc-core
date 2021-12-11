/*
 * EndpointEventBase.cpp
 *
 *  Created on: Dec 6, 2021
 *      Author: mballance
 */

#include "EndpointEventBase.h"

namespace tblink_rpc_core {

EndpointEventBase::EndpointEventBase(IEndpointEvent::kind_t kind) :
	m_kind(kind) {

}

EndpointEventBase::~EndpointEventBase() {
	// TODO Auto-generated destructor stub
}

} /* namespace tblink_rpc_core */
