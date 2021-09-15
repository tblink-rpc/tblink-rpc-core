/*
 * Type.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */
#pragma once
#include "tblink_rpc/IType.h"

namespace tblink_rpc_core {

class Type : public virtual IType {
public:
	Type(TypeE kind);

	virtual ~Type();

	virtual TypeE kind() const override {
		return m_kind;
	}

private:
	TypeE				m_kind;

};

} /* namespace tblink_rpc_core */

