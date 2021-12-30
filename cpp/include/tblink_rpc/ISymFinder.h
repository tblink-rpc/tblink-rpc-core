/*
 * IDynSymFinder.h
 *
 *  Created on: Oct 13, 2021
 *      Author: mballance
 */

#pragma once
#include <memory>
#include <string>

namespace tblink_rpc_core {

class ISymFinder;
using ISymFinderUP=std::unique_ptr<ISymFinder>;
class ISymFinder {
public:

	using result_t=std::pair<ISymFinderUP,std::string>;

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

