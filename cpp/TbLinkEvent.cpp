/*
 * TbLinkEvent.cpp
 *
 *  Created on: Jan 17, 2022
 *      Author: mballance
 */

#include "TbLinkEvent.h"

namespace tblink_rpc_core {

TbLinkEvent::TbLinkEvent(
		TbLinkEventKind		kind,
		void				*hndl) : m_kind(kind), m_hndl(hndl) {

}

TbLinkEvent::~TbLinkEvent() {
	// TODO Auto-generated destructor stub
}

} /* namespace tblink_rpc_core */
