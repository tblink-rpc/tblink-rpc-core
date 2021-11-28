/*
 * LaunchTypeNativeLoopback.cpp
 *
 *  Created on: Nov 27, 2021
 *      Author: mballance
 */

#include "LaunchParams.h"
#include "LaunchTypeNativeLoopback.h"

namespace tblink_rpc_core {

LaunchTypeNativeLoopback::LaunchTypeNativeLoopback() {
	// TODO Auto-generated constructor stub

}

LaunchTypeNativeLoopback::~LaunchTypeNativeLoopback() {
	// TODO Auto-generated destructor stub
}

ILaunchType::result_t LaunchTypeNativeLoopback::launch(ILaunchParams *params) {

}

ILaunchParams *LaunchTypeNativeLoopback::newLaunchParams() {
	return new LaunchParams();
}

} /* namespace tblink_rpc_core */
