/*
 * MethodTypeBuilder.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IMethodTypeBuilder.h"
#include "tblink_rpc/IType.h"
#include "MethodType.h"

namespace tblink_rpc_core {

class MethodTypeBuilder : public IMethodTypeBuilder {
public:

	MethodTypeBuilder(
			const std::string		&name,
			intptr_t				id,
			IType					*rtype,
			bool					is_export,
			bool					is_blocking);

	virtual ~MethodTypeBuilder();

	virtual void add_param(
			const std::string		&name,
			IType					*type) override;

	MethodType *method_t() {
		return m_method_t.release();
	}

private:
	MethodTypeUP					m_method_t;

};

} /* namespace tblink_rpc_core */

