/*
 * TypeIfc.cpp
 *
 *  Created on: Feb 6, 2022
 *      Author: mballance
 */

#include "TypeIfc.h"

namespace tblink_rpc_core {

TypeIfc::TypeIfc(
		const std::string		&name,
		bool					is_mirror) :
				m_name(name), m_is_mirror(is_mirror) {

}

TypeIfc::~TypeIfc() {
	// TODO Auto-generated destructor stub
}

} /* namespace tblink_rpc_core */
