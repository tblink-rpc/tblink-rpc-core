/*
 * JsonMethodType.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#include "MethodType.h"
#include "ParamDecl.h"

namespace tblink_rpc_core {

MethodType::MethodType(
		const std::string		&name,
		intptr_t				id,
		IType					*rtype,
		bool					is_export,
		bool					is_blocking) :
			m_name(name), m_id(id),
			m_rtype(rtype),
			m_is_export(is_export),
			m_is_blocking(is_blocking) {
	// TODO Auto-generated constructor stub

}

MethodType::~MethodType() {
	// TODO Auto-generated destructor stub
}

void MethodType::add_param(
			const std::string			&name,
			IType						*type) {
	ParamDecl *p = new ParamDecl(name, type);
	m_params.push_back(p);
	m_params_up.push_back(IParamDeclUP(p));
}

} /* namespace tblink_rpc_core */
