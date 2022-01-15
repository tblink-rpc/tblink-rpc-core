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

bool LaunchParams::has_param(
			const std::string		&key) {
	return m_params.find(key) != m_params.end();
}

std::string LaunchParams::get_param(
			const std::string		&key) {
	if (m_params.find(key) != m_params.end()) {
		return m_params.find(key)->second;
	} else {
		return "";
	}
}

} /* namespace tblink_rpc_core */
