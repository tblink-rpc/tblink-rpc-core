/*
 * IInterfaceInstFactory.h
 *
 *  Created on: Jan 15, 2022
 *      Author: mballance
 */

#pragma once
#include <string>

namespace tblink_rpc_core {

class IEndpoint;
class IInterfaceInst;
class IInterfaceType;

class IInterfaceInstFactory {
public:

	virtual ~IInterfaceInstFactory() { }

	virtual IInterfaceInst *createInterfaceInst(
			IEndpoint			*ep,
			IInterfaceType		*type,
			const std::string	&inst_name,
			bool				is_mirror
			) = 0;

	virtual IInterfaceInstFactory *clone() = 0;

};

}

