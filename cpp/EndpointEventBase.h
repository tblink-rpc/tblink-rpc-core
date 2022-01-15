/*
 * EndpointEventBase.h
 *
 *  Created on: Dec 6, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IEndpointEvent.h"

namespace tblink_rpc_core {

class EndpointEventBase : public IEndpointEvent {
public:
	EndpointEventBase(IEndpointEvent::kind_t kind);

	virtual ~EndpointEventBase();

	virtual IEndpointEvent::kind_t kind() const { return m_kind; }

protected:
	IEndpointEvent::kind_t				m_kind;
};

} /* namespace tblink_rpc_core */

