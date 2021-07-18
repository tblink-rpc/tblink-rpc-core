/*
 * IMethod.h
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */

#pragma once
#include <memory>

namespace tblink_rpc_core {

class IMethodType;
typedef std::unique_ptr<IMethodType> IMethodTypeUP;
class IMethodType {
public:

	virtual ~IMethodType() { }

	virtual const std::string &name() = 0;

	virtual intptr_t id() = 0;

	virtual const std::string &signature() = 0;

	virtual bool is_export() = 0;

	virtual bool is_blocking() = 0;


};

}
