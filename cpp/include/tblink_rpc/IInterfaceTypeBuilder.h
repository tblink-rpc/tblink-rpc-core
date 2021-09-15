/*
 * IInterfaceTypeBuilder.h
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */
#pragma once
#include "IMethodType.h"
#include "IMethodTypeBuilder.h"
#include "ITypeFactory.h"

namespace tblink_rpc_core {

class IInterfaceTypeBuilder : public virtual ITypeFactory {
public:

	virtual ~IInterfaceTypeBuilder() { }

	virtual IMethodTypeBuilder *newMethodTypeBuilder(
			const std::string	&name,
			intptr_t			id,
			IType				*rtype,
			bool				is_export,
			bool				is_blocking) = 0;

	/**
	 * signature is: <rtype>(<pn>=<pt>,...)
	 */
	virtual IMethodType *add_method(
			IMethodTypeBuilder	*mtb) = 0;

};

}
