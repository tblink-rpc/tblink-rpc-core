/*
 * TypeMap.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */
#pragma once
#include "tblink_rpc/ITypeMap.h"
#include "Type.h"

namespace tblink_rpc_core {

class TypeMap : public virtual ITypeMap {
public:
	TypeMap(
			IType		*key_t,
			IType		*val_t);

	virtual ~TypeMap();

	virtual TypeE kind() const override { return TypeE::Map; }

	virtual IType *key_t() const override {
		return m_key_t.get();
	}

	virtual IType *elem_t() const override {
		return m_elem_t.get();
	}

private:
	ITypeUP					m_key_t;
	ITypeUP					m_elem_t;

};

} /* namespace tblink_rpc_core */

