/*
 * TypeInt.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */
#pragma once
#include "tblink_rpc/ITypeInt.h"
#include "Type.h"

namespace tblink_rpc_core {

class TypeInt : /*public Type,*/ public virtual ITypeInt {
public:
	TypeInt(
			bool			is_signed,
			int32_t			width);

	virtual ~TypeInt();

	virtual TypeE kind() const override { return TypeE::Int; }

	virtual bool is_signed() const override {
		return m_is_signed;
	}

	virtual int32_t width() const override {
		return m_width;
	}

private:
	bool					m_is_signed;
	int32_t					m_width;
};

} /* namespace tblink_rpc_core */

