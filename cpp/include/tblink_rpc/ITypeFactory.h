/*
 * ITypeFactory.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */
#pragma once
#include "tblink_rpc/ITypeIfc.h"
#include "tblink_rpc/ITypeInt.h"
#include "tblink_rpc/ITypeMap.h"
#include "tblink_rpc/ITypeVec.h"

namespace tblink_rpc_core {

class ITypeFactory {
public:

	virtual ~ITypeFactory() { }

	virtual IType *mkTypeBool() = 0;

	virtual ITypeIfc *mkTypeIfc(
			const std::string	&name,
			bool				is_mirror) = 0;

	virtual ITypeInt *mkTypeInt(
			bool		is_signed,
			int32_t		width) = 0;

	virtual ITypeMap *mkTypeMap(
			IType		*key_t,
			IType		*elem_t) = 0;

	virtual IType *mkTypeStr() = 0;

	virtual ITypeVec *mkTypeVec(
			IType		*elem_t) = 0;

};

}
