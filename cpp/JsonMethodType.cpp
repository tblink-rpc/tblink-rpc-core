/*
 * JsonMethodType.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#include "JsonMethodType.h"

namespace tblink_rpc_core {

JsonMethodType::JsonMethodType(
		const std::string		&name,
		intptr_t				id,
		const std::string		&signature,
		bool					is_export,
		bool					is_blocking) :
			m_name(name), m_id(id),
			m_signature(signature),
			m_is_export(is_export),
			m_is_blocking(is_blocking) {
	// TODO Auto-generated constructor stub

}

JsonMethodType::~JsonMethodType() {
	// TODO Auto-generated destructor stub
}

} /* namespace tblink_rpc_core */
