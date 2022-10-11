/*
 * Env.h
 *
 *  Created on: Sep 19, 2021
 *      Author: mballance
 */

#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace tblink_rpc_core {

class Env {
public:
	Env();

	virtual ~Env();

	char **environ_p();

	bool has(
			const std::string		&key);

	std::string get(
			const std::string		&key);

	void set(
			const std::string		&key,
			const std::string		&var);

	void clr(const std::string		&key);

private:
	void release_environ_p();

private:
	std::map<std::string,std::string>		m_env;
	char									**m_environ_p;
	std::vector<char *>						m_env_vars;
};

} /* namespace tblink_rpc_core */

