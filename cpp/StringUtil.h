/*
 * StringUtil.h
 *
 *  Created on: Dec 29, 2021
 *      Author: mballance
 */

#pragma once
#include <string>
#include <vector>

namespace tblink_rpc_core {

class StringUtil {
public:
	StringUtil();

	virtual ~StringUtil();

	static std::vector<std::string> split_lines(
			const std::string &str);

	static std::vector<std::string> split_pylist(
			const std::string &str);
};

} /* namespace tblink_rpc_core */

