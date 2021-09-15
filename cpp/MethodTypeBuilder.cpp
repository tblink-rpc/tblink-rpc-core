/*
 * MethodTypeBuilder.cpp
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */

#include "MethodTypeBuilder.h"

namespace tblink_rpc_core {

MethodTypeBuilder::MethodTypeBuilder(
		const std::string			&name,
		intptr_t					id,
		IType						*rtype,
		bool						is_export,
		bool						is_blocking) : m_method_t(
				new MethodType(name, id, rtype, is_export, is_blocking)) {

}

MethodTypeBuilder::~MethodTypeBuilder() {
	// TODO Auto-generated destructor stub
}

void MethodTypeBuilder::add_param(
			const std::string		&name,
			IType					*type) {
	m_method_t->add_param(name, type);
}

} /* namespace tblink_rpc_core */
