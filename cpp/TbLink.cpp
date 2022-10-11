/*
 * Factory.cpp
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#include "TbLink.h"
#include <stdio.h>
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
	#include <dlfcn.h>
#endif
#include <string>
#include <unordered_set>
#include <sys/types.h>
#include <sys/stat.h>

#include "DynLibSymFinder.h"
#include "EndpointMsgBase.h"
#include "LaunchParams.h"
#include "TbLinkEvent.h"
#include "TransportJsonSocket.h"

namespace tblink_rpc_core {

TbLink::TbLink() : m_default_ep(0) {
	// TODO Auto-generated constructor stub

}

TbLink::~TbLink() {
	// TODO Auto-generated destructor stub
}

void TbLink::addListener(ITbLinkListener *l) {
	m_listeners.push_back(ITbLinkListenerUP(l));
}

void TbLink::removeListener(ITbLinkListener *l) {
	for (std::vector<ITbLinkListenerUP>::iterator
			it=m_listeners.begin();
			it!=m_listeners.end(); it++) {
		if (it->get() == l) {
			it->release();
			m_listeners.erase(it);
			break;
		}
	}
}

void TbLink::addEndpoint(IEndpoint *ep) {
	m_endpoints.push_back(ep);
	fprintf(stdout, "addEndpoint: %u listeners\n", (unsigned int)m_listeners.size());
	fflush(stdout);
	sendEvent(TbLinkEventKind::AddEndpoint, ep);
}

void TbLink::removeEndpoint(IEndpoint *ep) {
	for (std::vector<IEndpoint *>::iterator
			it=m_endpoints.begin();
			it!=m_endpoints.end(); it++) {
		if (*it == ep) {
			m_endpoints.erase(it);
			break;
		}
	}
	sendEvent(TbLinkEventKind::RemEndpoint, ep);
}

const std::vector<IEndpoint *> &TbLink::getEndpoints() const {
	return m_endpoints;
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
#ifdef _WIN32
	HMODULE hndl = LoadLibraryA(path.c_str());
#else
	void *hndl = dlopen(path.c_str(), RTLD_LAZY);
#endif

	if (hndl) {
		return {ISymFinderUP(new DynLibSymFinder(hndl)), ""};
	} else {
#ifdef _WIN32
		return {ISymFinderUP(), "TODO: Win32 Error"};
#else
		return {ISymFinderUP(), dlerror()};
#endif
	}
}

TbLink *TbLink::inst() {
	if (!m_inst) {
		m_inst = new TbLink();
	}
	return m_inst;
}

void TbLink::sendEvent(TbLinkEventKind kind, void *hndl) {
	TbLinkEvent ev(kind, hndl);

	for (auto it=m_listeners.begin(); it!=m_listeners.end(); it++) {
		(*it)->notify(&ev);
	}
}

TbLink *TbLink::m_inst = 0;

} /* namespace tblink_rpc_core */

extern "C" {
tblink_rpc_core::ITbLink *tblink() {
	return tblink_rpc_core::TbLink::inst();
}
}


