/*
 * Path.h
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#pragma once
#include <string>

class Path {
public:
	Path();
	virtual ~Path();

    static bool isdir(const std::string &path);

    static bool isfile(const std::string &path);

    static std::string abspath(const std::string &path);

    /**
     *
     */
    static std::string findpath(const std::string &path);

    static std::string join(const std::string &root, const std::string &leaf);

    static std::string join(std::initializer_list<std::string> elems);

    static std::string dirname(const std::string &path);

    static std::string basename(const std::string &path);

    static bool mkdir(const std::string &path);

    static bool rmdir(const std::string &path);

};

