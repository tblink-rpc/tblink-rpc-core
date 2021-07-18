/*
 * JsonInterfaceTypeBuilder.h
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#pragma once
#include "JsonInterfaceType.h"
#include "tblink_rpc/IInterfaceTypeBuilder.h"

namespace tblink_rpc_core {

class JsonInterfaceTypeBuilder : public IInterfaceTypeBuilder {
public:
	JsonInterfaceTypeBuilder(const std::string &name);

	virtual ~JsonInterfaceTypeBuilder();

	JsonInterfaceType *type(bool release=true) {
		return (release)?m_type.release():m_type.get();
	}

	virtual IMethodType *define_method(
			const std::string	&name,
			intptr_t			id,
			const std::string	&signature,
			bool				is_export,
			bool				is_blocking) override;

private:
	JsonInterfaceTypeUP				m_type;
};

} /* namespace tblink_rpc_core */

