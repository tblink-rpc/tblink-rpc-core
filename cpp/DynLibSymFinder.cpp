/*
 * DynLibSymFinder.cpp
 *
 *  Created on: Dec 29, 2021
 *      Author: mballance
 */

#include <dlfcn.h>
#include "DynLibSymFinder.h"

namespace tblink_rpc_core {

DynLibSymFinder::DynLibSymFinder(void *hndl) : m_hndl(hndl) {

}

DynLibSymFinder::~DynLibSymFinder() {
	// TODO Auto-generated destructor stub
}

void *DynLibSymFinder::findSym(const std::string &sym) {
	void *sym_h = dlsym(m_hndl, sym.c_str());

	if (!sym_h) {
		m_error = dlerror();
	}

	return sym_h;
}

std::pair<void *,std::string> DynLibSymFinder::findSymLib(
		const std::string		&sym) {
	// TODO: this doesn't really belong here
	return {0,""};
}

std::string DynLibSymFinder::error() {
	return m_error;
}

} /* namespace tblink_rpc_core */
