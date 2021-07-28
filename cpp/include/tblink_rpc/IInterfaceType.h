/*
 * IInterfaceType.h
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */
#pragma once
#include <vector>
#include "IMethodType.h"

namespace tblink_rpc_core {

class IInterfaceType;
typedef std::unique_ptr<IInterfaceType> IInterfaceTypeUP;
class IInterfaceType {
public:

	virtual ~IInterfaceType() { }

//	virtual int32_t id() = 0;

	virtual const std::string &name() = 0;

	virtual const std::vector<IMethodType *> &methods() = 0;

	virtual IMethodType *findMethod(const std::string &name) = 0;

//	virtual IMethodType *getMethodById(int32_t id) = 0;


};

}
