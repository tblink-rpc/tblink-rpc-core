/*
 * ITypeInt.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IType.h"

namespace tblink_rpc_core {

class ITypeInt : public virtual IType {
public:

	virtual ~ITypeInt() { }

	virtual bool is_signed() const = 0;

	virtual int32_t width() const = 0;

};

}
