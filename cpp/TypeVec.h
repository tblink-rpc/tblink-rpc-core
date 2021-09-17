/*
 * TypeVec.h
 *
 *  Created on: Sep 17, 2021
 *      Author: mballance
 */

#pragma once
#include "Type.h"

namespace tblink_rpc_core {

class TypeVec : public Type {
public:
	TypeVec(IType *elem_t);

	virtual ~TypeVec();

	IType *elem_t() const { return m_elem_t.get(); }

private:
	ITypeUP				m_elem_t;
};

} /* namespace tblink_rpc_core */

