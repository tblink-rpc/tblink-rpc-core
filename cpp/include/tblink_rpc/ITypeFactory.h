/*
 * ITypeFactory.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */
#pragma once
#include "ITypeInt.h"
#include "ITypeMap.h"
#include "ITypeVec.h"

namespace tblink_rpc_core {

class ITypeFactory {
public:

	virtual ~ITypeFactory() { }

	virtual ITypeInt *mkTypeBool() = 0;

	virtual ITypeInt *mkTypeInt(
			bool		is_signed,
			int32_t		width) = 0;

	virtual ITypeMap *mkTypeMap(
			IType		*key_t,
			IType		*elem_t) = 0;

	virtual ITypeInt *mkTypeStr() = 0;

	virtual ITypeVec *mkTypeVec(
			IType		*elem_t) = 0;

};

}
