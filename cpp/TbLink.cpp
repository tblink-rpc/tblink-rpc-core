/*
 * Factory.cpp
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#include "TbLink.h"
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <unordered_set>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "DynLibSymFinder.h"
#include "EndpointMsgBase.h"
#include "LaunchParams.h"
#include "TransportJsonSocket.h"

namespace tblink_rpc_core {

TbLink::TbLink() : m_default_ep(0) {
	// TODO Auto-generated constructor stub

}

TbLink::~TbLink() {
	// TODO Auto-generated destructor stub
}

void TbLink::addLaunchType(ILaunchType *launch_t) {
	m_launch_type_m.insert({launch_t->name(), launch_t});
	m_launch_types.push_back(launch_t);
}

ILaunchType *TbLink::findLaunchType(const std::string &id) {
	std::map<std::string,ILaunchType*>::const_iterator it;

	if ((it=m_launch_type_m.find(id)) != m_launch_type_m.end()) {
		return it->second;
	} else {
		return 0;
	}
}

const std::string &TbLink::getLibPath() {
	if (m_libpath == "") {
		m_sym_finder.init();
		std::pair<void *, std::string> path = m_sym_finder.findSymLib("tblink");

		m_libpath = path.second;

		if (m_libpath.size() == 0) {
			fprintf(stdout, "TBLink Error: failed to find library path\n");
		}
	}

	return m_libpath;
}

ISymFinder *TbLink::sym_finder() {
	m_sym_finder.init();
	return &m_sym_finder;
}

ISymFinder::result_t TbLink::load_library(
			const std::string &path) {
	void *hndl = dlopen(path.c_str(), RTLD_LAZY);

	if (hndl) {
		return {ISymFinderUP(new DynLibSymFinder(hndl)), ""};
	} else {
		return {ISymFinderUP(), dlerror()};
	}
}

TbLink *TbLink::inst() {
	if (!m_inst) {
		m_inst = new TbLink();
	}
	return m_inst;
}

TbLink *TbLink::m_inst = 0;

} /* namespace tblink_rpc_core */

extern "C" {
tblink_rpc_core::ITbLink *tblink() {
	return tblink_rpc_core::TbLink::inst();
}
}


