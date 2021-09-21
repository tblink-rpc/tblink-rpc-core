/*
 * LaunchParams.cpp
 *
 *  Created on: Sep 19, 2021
 *      Author: mballance
 */

#include "LaunchParams.h"

namespace tblink_rpc_core {

LaunchParams::LaunchParams() {
	// TODO Auto-generated constructor stub

}

LaunchParams::~LaunchParams() {
	// TODO Auto-generated destructor stub
}

void LaunchParams::add_arg(const std::string &arg) {
	m_args.push_back(arg);
}

void LaunchParams::add_param(
			const std::string		&key,
			const std::string		&val) {
	m_params.insert({key, val});
}

} /* namespace tblink_rpc_core */
