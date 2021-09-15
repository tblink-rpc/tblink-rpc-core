/*
 * IMethodTypeBuilder.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */

#pragma once
#include <string>
#include "tblink_rpc/IType.h"

namespace tblink_rpc_core {

class IMethodTypeBuilder {
public:

	virtual ~IMethodTypeBuilder() { }

	virtual void add_param(
			const std::string		&name,
			IType					*type) = 0;


};

}
