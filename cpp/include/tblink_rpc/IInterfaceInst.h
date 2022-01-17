/*
 * IInterfaceInst.h
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */

#pragma once

#include <functional>
#include <stdint.h>
#include "tblink_rpc/IInterfaceImpl.h"
#include "IInterfaceType.h"
#include "IMethodType.h"
#include "IParamValFactory.h"
#include "IParamValVec.h"

namespace tblink_rpc_core {

class IEndpoint;

class IInterfaceInst;
using IInterfaceInstUP=std::unique_ptr<IInterfaceInst>;
typedef std::function<void(
		IInterfaceInst		*inst,
		IMethodType			*method,
		intptr_t			call_id,
		IParamValVec		*params)> invoke_req_f;

typedef std::function<void(IParamVal *retval)> invoke_rsp_f;

class IInterfaceInst : public virtual IParamValFactory {
public:

	virtual ~IInterfaceInst() { }

	virtual IEndpoint *endpoint() const = 0;

	virtual const std::string &name() = 0;

	virtual IInterfaceType *type() = 0;

	virtual bool is_mirror() = 0;

	virtual void setImpl(IInterfaceImpl *impl) = 0;

	/**
	 * Invokes a method and delivers the response via a callback.
	 * Note that the response may be delivered while within the `invoke`
	 * call or after it returns.
	 */
	virtual int32_t invoke(
			IMethodType									*method,
			IParamValVec								*params,
			const invoke_rsp_f							&completion_f) = 0;

	/**
	 * Sends a response to an invocation
	 */
	virtual void invoke_rsp(
			intptr_t									call_id,
			IParamVal									*ret) = 0;


};

}
