/*
 * TypeMap.cpp
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */

#include "TypeMap.h"

namespace tblink_rpc_core {

TypeMap::TypeMap(
		IType			*key_t,
		IType			*elem_t) : Type(TypeE::Map),
				m_key_t(key_t), m_elem_t(elem_t) {
	// TODO Auto-generated constructor stub

}

TypeMap::~TypeMap() {
	// TODO Auto-generated destructor stub
}

} /* namespace tblink_rpc_core */
