/*
 * JsonInterfaceTypeBuilder.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#include "InterfaceTypeBuilder.h"
#include "MethodType.h"
#include "MethodTypeBuilder.h"
#include "Type.h"
#include "TypeInt.h"
#include "TypeMap.h"
#include "TypeVec.h"

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
	fprintf(stdout, "--> newMethodTypeBuilder\n");
	MethodTypeBuilder *mtb = new MethodTypeBuilder(
			name,
			id,
			rtype,
			is_export,
			is_blocking);
	fprintf(stdout, "<-- newMethodTypeBuilder %p\n", mtb);

	return mtb;
}

IMethodType *InterfaceTypeBuilder::add_method(
		IMethodTypeBuilder			*mtb) {
	IMethodType *mt = dynamic_cast<MethodTypeBuilder *>(mtb)->method_t();
	delete mtb;
	m_type->addMethod(mt);
	return mt;
}

IType *InterfaceTypeBuilder::mkTypeBool() {
	return new Type(TypeE::Bool);
}

ITypeInt *InterfaceTypeBuilder::mkTypeInt(
		bool		is_signed,
		int32_t		width) {
	return new TypeInt(is_signed, width);
}

ITypeMap *InterfaceTypeBuilder::mkTypeMap(
		IType		*key_t,
		IType		*elem_t) {
	return new TypeMap(key_t, elem_t);
}

IType *InterfaceTypeBuilder::mkTypeStr() {
	return new Type(TypeE::Str);
}

ITypeVec *InterfaceTypeBuilder::mkTypeVec(
		IType		*elem_t) {
	return 0;
}


} /* namespace tblink_rpc_core */
