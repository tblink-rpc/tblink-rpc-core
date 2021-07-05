/*
 * IInterfaceTypeBuilder.h
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */
#include "IMethodType.h"

namespace tblink_rpc_core {

class IInterfaceTypeBuilder {
public:

	virtual ~IInterfaceTypeBuilder() { }

	/**
	 * signature is: <rtype>(<pn>=<pt>,...)
	 */
	virtual IMethodType *define_method(
			const std::string	&name,
			intptr_t			id,
			const std::string	&signature,
			bool				is_export,
			bool				is_blocking) = 0;

};

}
