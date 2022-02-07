/*
 * ITypeIfc.h
 *
 *  Created on: Feb 6, 2022
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IType.h"

namespace tblink_rpc_core {

class ITypeIfc : public IType {
public:
	ITypeIfc();

	virtual ~ITypeIfc();

	virtual const std::string name() const = 0;

	virtual bool is_mirror() const = 0;

};

} /* namespace tblink_rpc_core */

