/*
 * ILaunchParams.h
 *
 *  Created on: Sep 19, 2021
 *      Author: mballance
 */

#pragma once
#include <map>
#include <string>
#include <vector>

namespace tblink_rpc_core {

class ILaunchParams {
public:

	virtual ~ILaunchParams() { }

	virtual void add_arg(const std::string &arg) = 0;

	virtual const std::vector<std::string> &args() const = 0;

	virtual void add_param(
			const std::string		&key,
			const std::string		&val) = 0;

	virtual bool has_param(
			const std::string		&key) = 0;

	virtual std::string get_param(
			const std::string		&key) = 0;

	virtual const std::map<std::string,std::string> &params() const = 0;


};

}
