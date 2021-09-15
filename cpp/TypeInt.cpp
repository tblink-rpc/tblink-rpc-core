/*
 * TypeInt.cpp
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */

#include "TypeInt.h"

namespace tblink_rpc_core {

TypeInt::TypeInt(
		bool			is_signed,
		int32_t			width) : Type(TypeE::Int),
				m_is_signed(is_signed), m_width(width) {

}

TypeInt::~TypeInt() {
	// TODO Auto-generated destructor stub
}

} /* namespace tblink_rpc_core */
