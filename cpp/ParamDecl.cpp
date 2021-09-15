/*
 * ParamDecl.cpp
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */

#include "ParamDecl.h"

namespace tblink_rpc_core {

ParamDecl::ParamDecl(
		const std::string		&name,
		IType					*type) : m_name(name), m_type(type) {
	// TODO Auto-generated constructor stub

}

ParamDecl::~ParamDecl() {
	// TODO Auto-generated destructor stub
}

} /* namespace tblink_rpc_core */
