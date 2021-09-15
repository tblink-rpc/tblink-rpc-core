/*
 * Env.h
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#pragma once
#include <map>
#include <string>


class Env {
public:
        Env();

        virtual ~Env();

        void unset(const std::string &var);

        void set(const std::string &var, const std::string &val);

        std::string get(const std::string &var);

        char **envp();

private:
        std::map<std::string,std::string>               m_env;
        char                                                                    **m_envp;

};


