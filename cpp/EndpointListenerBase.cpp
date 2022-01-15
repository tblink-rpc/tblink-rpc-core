/*
 * EndpointListenerBase.cpp
 *
 *  Created on: Dec 6, 2021
 *      Author: mballance
 */

#include "EndpointListenerBase.h"

namespace tblink_rpc_core {

EndpointListenerBase::EndpointListenerBase(const endpoint_ev_f &ev_f) :
		m_ev_f(ev_f) {

}

EndpointListenerBase::~EndpointListenerBase() {
	// TODO Auto-generated destructor stub
}

void EndpointListenerBase::event(const IEndpointEvent *ev) {
	m_ev_f(ev);
}

} /* namespace tblink_rpc_core */
