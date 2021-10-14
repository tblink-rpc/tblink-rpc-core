/*
 * IDynSymFinder.h
 *
 *  Created on: Oct 13, 2021
 *      Author: mballance
 */

#pragma once
#include <string>

namespace tblink_rpc_core {

class ISymFinder {
public:

	virtual ~ISymFinder() { }

	virtual void *findSym(const std::string &sym) = 0;

	template <class T> T findSymT(const std::string &sym) {
		return reinterpret_cast<T>(findSym(sym));
	}

	virtual std::pair<void *,std::string> findSymLib(
			const std::string		&sym) = 0;

	virtual std::string error() = 0;

};

}

