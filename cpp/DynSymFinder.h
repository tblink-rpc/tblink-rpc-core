/*
 * DynSymFinder.h
 *
 *  Created on: Oct 13, 2021
 *      Author: mballance
 */

#pragma once
#include <string>
#include <vector>
#include "tblink_rpc/ISymFinder.h"

namespace tblink_rpc_core {

/**
 * Finds dynamic symbols by looking at the libraries
 * loaded by the process
 */
class DynSymFinder : public virtual ISymFinder {
public:
	DynSymFinder();

	virtual ~DynSymFinder();

	int init();

	virtual void *findSym(const std::string &sym) override;

	template <class T> T findSymT(const std::string &sym) {
		return reinterpret_cast<T>(findSym(sym));
	}

	virtual std::pair<void *,std::string> findSymLib(
			const std::string		&sym) override;

	virtual std::string error() override;

private:
	bool										m_is_init;
	std::vector<std::string>					m_lib_paths;
	void										*m_last_lib;
	std::string									m_last_lib_path;
	std::vector<std::pair<std::string,void*>>	m_search_path;
};

} /* namespace tblink_rpc_core */

