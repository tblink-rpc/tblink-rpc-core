/*
 * StringUtil.cpp
 *
 *  Created on: Dec 29, 2021
 *      Author: mballance
 */

#include <stdio.h>
#include "StringUtil.h"

namespace tblink_rpc_core {

StringUtil::StringUtil() {
	// TODO Auto-generated constructor stub

}

StringUtil::~StringUtil() {
	// TODO Auto-generated destructor stub
}

std::vector<std::string> StringUtil::split_lines(
			const std::string &str) {
	std::vector<std::string> ret;
	int32_t start_idx = 0;
	int32_t end_idx = 0;

	while (end_idx < str.size()) {
		while (end_idx < str.size() && str.at(end_idx) != '\n') {
			end_idx++;
		}
//		if (end_idx < str.size()) {
			ret.push_back(str.substr(start_idx, end_idx));
//		} else {
//			ret.push_back(str.substr(start_idx));
//		}
		end_idx++;
		start_idx = end_idx;
	}

	return ret;
}

std::vector<std::string> StringUtil::split_pylist(
			const std::string &str) {
	std::vector<std::string> ret;
	int idx = 0;

	// Skip leading whitespace
	while (idx < str.size() && isspace(str.at(idx))) {
		idx++;
	}

	if (idx < str.size() && str.at(idx) == '[') {
		idx++;
	}

	// We're now inside braces
	while (idx < str.size()) {

		// Skip up to quote or closing brace
		while (idx < str.size() && str.at(idx) != '\'' && str.at(idx) != '"' && str.at(idx) != ']') {
			idx++;
		}

		if (idx >= str.size() || str.at(idx) == ']') {
			break;
		}

		char qc = str.at(idx);
		idx++; // Skip quote

		int32_t start_idx = idx;
		while (idx < str.size() && str.at(idx) != qc) {
			idx++;
		}

		if (start_idx == idx) {
			ret.push_back("");
		} else {
			ret.push_back(str.substr(start_idx, (idx-start_idx)));
		}

		idx++;
	}

	return ret;
}

} /* namespace tblink_rpc_core */
