/*
 * JsonInterfaceTypeBuilder.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#include "JsonInterfaceTypeBuilder.h"
#include "JsonMethodType.h"

namespace tblink_rpc_core {

JsonInterfaceTypeBuilder::JsonInterfaceTypeBuilder(const std::string &name) :
	m_type(new JsonInterfaceType(name)) {
	// TODO Auto-generated constructor stub

}

JsonInterfaceTypeBuilder::~JsonInterfaceTypeBuilder() {
	// TODO Auto-generated destructor stub
}

IMethodType *JsonInterfaceTypeBuilder::define_method(
			const std::string	&name,
			intptr_t			id,
			const std::string	&signature,
			bool				is_export,
			bool				is_blocking) {
	JsonMethodType *method = new JsonMethodType(
			name,
			id,
			signature,
			is_export,
			is_blocking);
	m_type->addMethod(method);

	return method;
}

} /* namespace tblink_rpc_core */
