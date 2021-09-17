/*
 * ITypeMap.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */
#pragma once

#include "IType.h"

namespace tblink_rpc_core {

class ITypeMap : public virtual IType {
public:

	virtual ~ITypeMap() { }

	virtual IType *key_t() const = 0;

	virtual IType *elem_t() const = 0;

};

}

