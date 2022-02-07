/*
 * JsonInterfaceTypeBuilder.h
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#pragma once
#include "InterfaceType.h"
#include "tblink_rpc/IInterfaceTypeBuilder.h"
#include "tblink_rpc/IMethodTypeBuilder.h"

namespace tblink_rpc_core {

class InterfaceTypeBuilder : public IInterfaceTypeBuilder {
public:
	InterfaceTypeBuilder(const std::string &name);

	virtual ~InterfaceTypeBuilder();

	InterfaceType *type(bool release=true) {
		return (release)?m_type.release():m_type.get();
	}

	virtual IMethodTypeBuilder *newMethodTypeBuilder(
			const std::string	&name,
			intptr_t			id,
			IType				*rtype,
			bool				is_export,
			bool				is_blocking) override;

	virtual IMethodType *add_method(
			IMethodTypeBuilder	*mtb) override;

	virtual IType *mkTypeBool() override;

	virtual ITypeIfc *mkTypeIfc(
			const std::string	&name,
			bool				is_mirror) override;

	virtual ITypeInt *mkTypeInt(
			bool		is_signed,
			int32_t		width) override;

	virtual ITypeMap *mkTypeMap(
			IType		*key_t,
			IType		*elem_t) override;

	virtual IType *mkTypeStr() override;

	virtual ITypeVec *mkTypeVec(
			IType		*elem_t) override;


private:
	InterfaceTypeUP				m_type;
};

} /* namespace tblink_rpc_core */

