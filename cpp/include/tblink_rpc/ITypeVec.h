/*
 * ITypeVec.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */
#pragma once
#include "IType.h"

namespace tblink_rpc_core {

class ITypeVec : public virtual IType {
public:

	virtual IType *elem_t() const = 0;

};

}
