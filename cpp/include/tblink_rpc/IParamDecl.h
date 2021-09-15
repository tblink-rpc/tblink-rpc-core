/*
 * IParamDecl.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */
#pragma once
#include <string>
#include "tblink_rpc/IType.h"

namespace tblink_rpc_core {

class IParamDecl;
typedef std::unique_ptr<IParamDecl> IParamDeclUP;
class IParamDecl {
public:

	virtual ~IParamDecl() { }

	virtual const std::string &name() const = 0;

	virtual IType *type() const = 0;

};

}

