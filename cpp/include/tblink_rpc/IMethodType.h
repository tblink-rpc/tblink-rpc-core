/*
 * IMethod.h
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */

#pragma once
#include <memory>
#include <vector>
#include "tblink_rpc/IParamDecl.h"
#include "tblink_rpc/IType.h"

namespace tblink_rpc_core {

class IMethodType;
typedef std::unique_ptr<IMethodType> IMethodTypeUP;
class IMethodType {
public:

	virtual ~IMethodType() { }

	virtual const std::string &name() = 0;

	virtual intptr_t id() = 0;

	virtual IType *rtype() const = 0;

	virtual bool is_export() = 0;

	virtual bool is_blocking() = 0;

	virtual const std::vector<IParamDecl *> &params() const = 0;

};

}
