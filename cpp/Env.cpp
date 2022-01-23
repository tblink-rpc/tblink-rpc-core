/*
 * Env.cpp
 *
 *  Created on: Sep 19, 2021
 *      Author: mballance
 */

#include "Env.h"
#include <string>
#include <string.h>

extern char **environ;

namespace tblink_rpc_core {

Env::Env() : m_environ_p(0) {
	// Initialize from the environment

	for (uint32_t i=0; environ[i]; i++) {
		std::string env(environ[i]);
		int32_t eq = env.find('=');
		if (eq != std::string::npos) {
			std::string key = env.substr(0, eq);
			std::string val = env.substr(eq+1);
			m_env.insert({key, val});
		}
	}
}

Env::~Env() {
	release_environ_p();
}

char **Env::environ_p() {
	if (!m_environ_p) {
		// Need to repopulate
		char **environ_p = new char *[m_env.size()+1];
		for (std::map<std::string,std::string>::const_iterator
				it=m_env.begin();
				it!=m_env.end(); it++) {
			char *var = new char[it->first.size()+it->second.size()+2];
			strcpy(var, it->first.c_str());
			strcat(var, "=");
			strcat(var, it->second.c_str());
			environ_p[m_env_vars.size()] = var;
			m_env_vars.push_back(var);
		}
		environ_p[m_env_vars.size()] = 0;
		m_environ_p = environ_p;
	}
	return m_environ_p;
}

bool Env::has(
			const std::string		&key) {
	return (m_env.find(key) != m_env.end());
}

std::string Env::get(
			const std::string		&key) {
	std::map<std::string,std::string>::const_iterator it;

	if ((it=m_env.find(key)) != m_env.end()) {
		return it->second;
	} else {
		return "";
	}
}

void Env::set(
			const std::string		&key,
			const std::string		&var) {
	release_environ_p();
	std::map<std::string,std::string>::const_iterator it;

	if ((it=m_env.find(key)) != m_env.end()) {
		m_env.erase(it);
	}
	m_env.insert({key, var});
}

void Env::clr(const std::string &key) {
	release_environ_p();
	std::map<std::string,std::string>::const_iterator it;

	if ((it=m_env.find(key)) != m_env.end()) {
		m_env.erase(it);
	}
}

void Env::release_environ_p() {
	if (m_environ_p) {
		delete [] m_environ_p;
	}
	m_environ_p = 0;
	for (std::vector<char *>::const_iterator
			it=m_env_vars.begin();
			it!=m_env_vars.end(); it++) {
		delete [] *it;
	}
	m_env_vars.clear();
}

} /* namespace tblink_rpc_core */
