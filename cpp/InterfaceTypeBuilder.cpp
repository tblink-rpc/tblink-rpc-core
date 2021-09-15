/*
 * JsonInterfaceTypeBuilder.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#include "InterfaceTypeBuilder.h"
#include "MethodType.h"
#include "MethodTypeBuilder.h"

namespace tblink_rpc_core {

InterfaceTypeBuilder::InterfaceTypeBuilder(const std::string &name) :
	m_type(new InterfaceType(name)) {
	// TODO Auto-generated constructor stub

}

InterfaceTypeBuilder::~InterfaceTypeBuilder() {
	// TODO Auto-generated destructor stub
}

IMethodTypeBuilder *InterfaceTypeBuilder::newMethodTypeBuilder(
			const std::string	&name,
			intptr_t			id,
			IType				*rtype,
			bool				is_export,
			bool				is_blocking) {
	MethodTypeBuilder *mtb = new MethodTypeBuilder(
			name,
			id,
			rtype,
			is_export,
			is_blocking);

	return mtb;
}

IMethodType *InterfaceTypeBuilder::add_method(
		IMethodTypeBuilder			*mtb) {

}

ITypeInt *InterfaceTypeBuilder::mkTypeBool() {

}

ITypeInt *InterfaceTypeBuilder::mkTypeInt(
		bool		is_signed,
		int32_t		width) {

}

ITypeMap *InterfaceTypeBuilder::mkTypeMap(
		IType		*key_t,
		IType		*elem_t) {

}

ITypeInt *InterfaceTypeBuilder::mkTypeStr() {

}

ITypeVec *InterfaceTypeBuilder::mkTypeVec(
		IType		*elem_t) {

}


} /* namespace tblink_rpc_core */
