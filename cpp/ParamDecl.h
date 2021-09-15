/*
 * ParamDecl.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IParamDecl.h"

namespace tblink_rpc_core {

class ParamDecl : public virtual IParamDecl {
public:
	ParamDecl(
			const std::string		&name,
			IType					*type);

	virtual ~ParamDecl();

	virtual const std::string &name() const {
		return m_name;
	}

	virtual IType *type() const {
		return m_type.get();
	}

private:
	std::string					m_name;
	ITypeUP						m_type;
};

} /* namespace tblink_rpc_core */

