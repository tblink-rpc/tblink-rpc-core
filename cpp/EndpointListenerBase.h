/*
 * EndpointListenerBase.h
 *
 *  Created on: Dec 6, 2021
 *      Author: mballance
 */

#pragma once
#include <functional>
#include "tblink_rpc/IEndpointListener.h"

namespace tblink_rpc_core {

using endpoint_ev_f=std::function<void(const IEndpointEvent *)>;

class EndpointListenerBase : public IEndpointListener {
public:
	EndpointListenerBase(const endpoint_ev_f &ev_f);

	virtual ~EndpointListenerBase();

	virtual void event(const IEndpointEvent *ev) override;

private:
	endpoint_ev_f					m_ev_f;
};

} /* namespace tblink_rpc_core */

