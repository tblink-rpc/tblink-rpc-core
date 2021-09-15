/*
 * JsonMethodType.h
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IMethodType.h"
#include "tblink_rpc/IParamDecl.h"
#include "tblink_rpc/IType.h"

namespace tblink_rpc_core {

class MethodType;
typedef std::unique_ptr<MethodType> MethodTypeUP;
class MethodType : public IMethodType {
public:
	MethodType(
			const std::string 	&name,
			intptr_t			id,
			IType				*rtype,
			bool				is_export,
			bool				is_blocking
			);

	virtual ~MethodType();

	virtual const std::string &name() override {
		return m_name;
	}

	virtual intptr_t id() override {
		return m_id;
	}

	virtual IType *rtype() const override {
		return m_rtype.get();
	}

	virtual bool is_export() override { return m_is_export; }

	virtual bool is_blocking() override { return m_is_blocking; }

	virtual const std::vector<IParamDecl *> &params() const override {
		return m_params;
	}

	void add_param(
			const std::string			&name,
			IType						*type);

private:
	std::string					m_name;
	intptr_t					m_id;
	ITypeUP						m_rtype;
	std::vector<IParamDecl *>	m_params;
	std::vector<IParamDeclUP>	m_params_up;
	bool						m_is_export;
	bool						m_is_blocking;

};

} /* namespace tblink_rpc_core */

