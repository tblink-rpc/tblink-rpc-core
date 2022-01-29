/*
 * TypeVec.h
 *
 *  Created on: Sep 17, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/ITypeVec.h"

namespace tblink_rpc_core {

class TypeVec : public virtual ITypeVec {
public:
	TypeVec(IType *elem_t);

	virtual ~TypeVec();

	virtual TypeE kind() const override { return TypeE::Vec; }

	IType *elem_t() const override { return m_elem_t.get(); }

private:
	ITypeUP				m_elem_t;
};

} /* namespace tblink_rpc_core */

