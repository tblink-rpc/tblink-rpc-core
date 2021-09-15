/*
 * Env.cpp
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#include "Env.h"
#include <unistd.h>
#include <string.h>
#include <string>


Env::Env() : m_envp(0) {
        char **p = environ;

        while (*p) {
                std::string v(*p);
                int32_t ep = v.find('=');
                std::string key = v.substr(0, ep);
                std::string val = v.substr(ep+1);

                m_env.insert({key, val});

                p++;
        }
}

Env::~Env() {
        // TODO Auto-generated destructor stub
        if (m_envp) {
                delete [] m_envp;
                m_envp = 0;
        }
}

void Env::unset(const std::string &var) {
        std::map<std::string,std::string>::iterator it;
        if ((it=m_env.find(var)) != m_env.end()) {
                m_env.erase(it);
        }
        if (m_envp) {
                delete [] m_envp;
                m_envp = 0;
        }
}

void Env::set(const std::string &var, const std::string &val) {
        std::map<std::string,std::string>::iterator it;
        if ((it=m_env.find(var)) != m_env.end()) {
                m_env.erase(it);
        }
        m_env.insert({var, val});
        if (m_envp) {
                delete [] m_envp;
                m_envp = 0;
        }
}

std::string Env::get(const std::string &var) {
	if (m_env.find(var) != m_env.end()) {
		return m_env.find(var)->second;
	} else {
		return "";
	}
}

char **Env::envp() {
        if (!m_envp) {
                m_envp = new char *[m_env.size()+1];
                char **p = m_envp;
                for (std::map<std::string,std::string>::iterator
                                it=m_env.begin(); it!=m_env.end(); it++) {
                        char *val = new char[it->first.size()+it->second.size()+2];
                        memcpy(val, it->first.c_str(), it->first.size());
                        val[it->first.size()] = '=';
                        memcpy(&val[it->first.size()+1], it->second.c_str(), it->second.size());
                        val[it->first.size()+it->second.size()+1] = 0;
                        *p = val;
                        p++;
                }
                // Terminate the array
                *p = 0;
        }

        return m_envp;
}



