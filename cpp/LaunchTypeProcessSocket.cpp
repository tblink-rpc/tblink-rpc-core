/*
 * LaunchTypeProcessSocket.cpp
 *
 *  Created on: Sep 19, 2021
 *      Author: mballance
 */

#include "LaunchTypeProcessSocket.h"
#include "LaunchTypeRegistration.h"

namespace tblink_rpc_core {

LaunchTypeProcessSocket::LaunchTypeProcessSocket() {
	// TODO Auto-generated constructor stub

}

LaunchTypeProcessSocket::~LaunchTypeProcessSocket() {
	// TODO Auto-generated destructor stub
}

static LaunchTypeRegistration<LaunchTypeProcessSocket>	m_registration;

} /* namespace tblink_rpc_core */
