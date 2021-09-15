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

class TypeInt : public virtual Type, public virtual ITypeInt {
public:
	TypeInt(
			bool			is_signed,
			int32_t			width);

	virtual ~TypeInt();

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

