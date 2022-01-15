/*
 * DynLibSymFinder.h
 *
 *  Created on: Dec 29, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/ISymFinder.h"
#include <dlfcn.h>

namespace tblink_rpc_core {

class DynLibSymFinder : public ISymFinder {
public:
	DynLibSymFinder(void *hndl);

	virtual ~DynLibSymFinder();

	virtual void *findSym(const std::string &sym) override;

	virtual std::pair<void *,std::string> findSymLib(
			const std::string		&sym) override;

	virtual std::string error() override;

private:
	void				*m_hndl;
	std::string			m_error;
};

} /* namespace tblink_rpc_core */

