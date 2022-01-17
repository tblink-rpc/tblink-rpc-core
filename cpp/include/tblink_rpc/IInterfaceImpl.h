/*
 * IInterfaceImpl.h
 *
 *  Created on: Jan 15, 2022
 *      Author: mballance
 */
#pragma once
#include <memory>
#include <stdint.h>

namespace tblink_rpc_core {

class IInterfaceInst;
class IMethodType;
class IParamValVec;

class IInterfaceImpl;
using IInterfaceImplUP=std::unique_ptr<IInterfaceImpl>;
class IInterfaceImpl {
public:

	virtual ~IInterfaceImpl() { }

	virtual void invoke(
			IInterfaceInst		*ifinst,
			IMethodType			*method,
			intptr_t			call_id,
			IParamValVec		*params) = 0;

};

}

