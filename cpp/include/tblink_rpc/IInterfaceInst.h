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
#include "IParamValVector.h"

namespace tblink_rpc_core {

class IInterfaceInst;
typedef std::function<void(
		IInterfaceInst		*inst,
		IMethodType			*method,
		intptr_t			call_id,
		IParamValVectorSP	params)> invoke_req_f;

class IInterfaceInst {
public:

	virtual ~IInterfaceInst() { }

	virtual IInterfaceType *type() = 0;

	virtual int32_t invoke(
			IMethodType									*method,
			IParamValVectorSP							params,
			const std::function<int32_t(IParamValSP)>	&completion_f) = 0;

	virtual void respond(
			uintptr_t									call_id,
			IParamValSP									ret);


	/*
	virtual int32_t invoke_nb(
			int32_t		 								method_id,
			IParamValVectorSP							params) = 0;
     */

};

}
