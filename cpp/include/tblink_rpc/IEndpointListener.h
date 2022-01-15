/*
 * IEndpointListener.h
 *
 *  Created on: Sep 16, 2021
 *      Author: mballance
 */

#pragma once
#include <functional>
#include <memory>
#include "tblink_rpc/IEndpointEvent.h"

namespace tblink_rpc_core {

using endpoint_ev_f=std::function<void(const IEndpointEvent *)>;

class IEndpointListener;
using IEndpointListenerUP=std::unique_ptr<IEndpointListener>;

class IEndpointListener {
public:

	virtual ~IEndpointListener() { }

	virtual void event(const IEndpointEvent *ev) = 0;

};

}
