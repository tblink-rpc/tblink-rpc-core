/*
 * LaunchParams.h
 *
 *  Created on: Sep 19, 2021
 *      Author: mballance
 */
#pragma once
#include <map>
#include <string>
#include <vector>
#include "tblink_rpc/ILaunchParams.h"

namespace tblink_rpc_core {

class LaunchParams : public ILaunchParams {
public:
	LaunchParams();

	virtual ~LaunchParams();

	virtual void add_arg(const std::string &arg) override;

	virtual const std::vector<std::string> &args() const {
		return m_args;
	}

	virtual void add_param(
			const std::string		&key,
			const std::string		&val) override;

	virtual bool has_param(
			const std::string		&key) override;

	virtual std::string get_param(
			const std::string		&key) override;

	virtual const std::map<std::string,std::string> &params() const {
		return m_params;
	}

private:
	std::vector<std::string>					m_args;
	std::map<std::string,std::string>			m_params;
};

} /* namespace tblink_rpc_core */

