/*
 * IEndpoint.h
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */

#pragma once
#include <functional>
#include <string>
//#include "IType.h"
//#include "ITypeBuilder.h"
#include "tblink_rpc/IInterfaceInst.h"
#include "tblink_rpc/IInterfaceType.h"
#include "tblink_rpc/IInterfaceTypeBuilder.h"

namespace tblink_rpc_core {

class IEndpoint;
typedef std::unique_ptr<IEndpoint> IEndpointUP;
class IEndpoint {
public:

	virtual ~IEndpoint() { }

	virtual bool build_complete() = 0;

	virtual bool connect_complete() = 0;

	virtual const std::string &last_error() = 0;

	/*
	virtual ITypeBuilderSP mk_type_builder(
			const std::string		&name) = 0;

	virtual ITypeSP define_interface_type(
			ITypeBuilderSP			type) = 0;
			 */

	virtual IInterfaceTypeBuilder *newInterfaceTypeBuilder(
			const std::string		&name) = 0;

	virtual IInterfaceType *defineInterfaceType(
			IInterfaceTypeBuilder	*type) = 0;

	virtual IInterfaceInst *defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			const invoke_req_f		&req_f) = 0;

	// TODO: register type
	//       - could have an object-lifetime manager attached

	// TODO: register static instance

};

}

