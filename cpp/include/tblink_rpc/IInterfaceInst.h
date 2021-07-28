/*
 * IInterfaceInst.h
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */

#pragma once

#include <functional>
#include <stdint.h>
#include "IInterfaceType.h"
#include "IMethodType.h"
#include "IParamValFactory.h"
#include "IParamValVector.h"

namespace tblink_rpc_core {

class IInterfaceInst;
typedef std::function<void(
		IInterfaceInst		*inst,
		IMethodType			*method,
		intptr_t			call_id,
		IParamValVectorSP	params)> invoke_req_f;

typedef std::function<void(IParamValSP retval)> invoke_rsp_f;

class IInterfaceInst : public virtual IParamValFactory {
public:

	virtual ~IInterfaceInst() { }

	virtual const std::string &name() = 0;

	virtual IInterfaceType *type() = 0;

	/**
	 * Invokes a blocking method via the peer endpoint
	 */
	virtual int32_t invoke(
			IMethodType									*method,
			IParamValVectorSP							params,
			const invoke_rsp_f							&completion_f) = 0;

	/**
	 * Invoke a non-blocking method via the peer endpoint
	 */
	virtual IParamValSP invoke_nb(
			IMethodType									*method,
			IParamValVectorSP							params) = 0;

	/**
	 * Sends a response to an invocation
	 */
	virtual void invoke_rsp(
			intptr_t									call_id,
			IParamValSP									ret) = 0;


};

}
