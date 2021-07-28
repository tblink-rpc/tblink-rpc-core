/*
 * JsonMethodType.h
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IMethodType.h"

namespace tblink_rpc_core {

class JsonMethodType;
typedef std::unique_ptr<JsonMethodType> JsonMethodTypeUP;
typedef std::shared_ptr<JsonMethodType> JsonMethodTypeSP;
class JsonMethodType : public IMethodType {
public:
	JsonMethodType(
			const std::string 	&name,
			intptr_t			id,
			const std::string	&signature,
			bool				is_export,
			bool				is_blocking
			);

	virtual ~JsonMethodType();

	virtual const std::string &name() override {
		return m_name;
	}

	virtual intptr_t id() override {
		return m_id;
	}

	virtual const std::string &signature() {
		return m_signature;
	}

	virtual bool is_export() override { return m_is_export; }

	virtual bool is_blocking() override { return m_is_blocking; }

private:
	std::string					m_name;
	intptr_t					m_id;
	std::string					m_signature;
	bool						m_is_export;
	bool						m_is_blocking;

};

} /* namespace tblink_rpc_core */

