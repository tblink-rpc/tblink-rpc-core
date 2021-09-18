/*
 * TypeVec.cpp
 *
 *  Created on: Sep 17, 2021
 *      Author: mballance
 */

#include "TypeVec.h"

namespace tblink_rpc_core {

TypeVec::TypeVec(IType *elem_t) : Type(TypeE::Vec), m_elem_t(elem_t) {
	// TODO Auto-generated constructor stub

}

TypeVec::~TypeVec() {
	// TODO Auto-generated destructor stub
}

} /* namespace tblink_rpc_core */
