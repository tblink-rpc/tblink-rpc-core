/*
 * DynSymFinder.cpp
 *
 *  Created on: Oct 13, 2021
 *      Author: mballance
 */

#include "DynSymFinder.h"
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#include <dlfcn.h>
#else
#include <windows.h>
#endif
#include <string>
#include <unordered_set>
#include <sys/types.h>
#include <sys/stat.h>

namespace tblink_rpc_core {

DynSymFinder::DynSymFinder() : m_is_init(false), m_last_lib(0) {
	// TODO Auto-generated constructor stub

}

DynSymFinder::~DynSymFinder() {
	// TODO Auto-generated destructor stub
}

int DynSymFinder::init() {
	if (m_is_init) {
		return 0;
	}

	/**
	 * Find the libraries loaded by the process
	 */
	pid_t pid = getpid();
	char mapfile_path[128];

	sprintf(mapfile_path, "/proc/%d/maps", pid);
	FILE *map_fp = fopen(mapfile_path, "r");

	std::unordered_set<std::string> so_files;
	while (fgets(mapfile_path, sizeof(mapfile_path), map_fp)) {
		std::string path = mapfile_path;
		int32_t idx;

		if ((idx=path.find('/')) != std::string::npos) {
			int32_t eidx = path.size()-1;
			while (isspace(path.at(eidx))) {
				eidx--;
			}
			path = path.substr(idx, (eidx-idx+1));

			struct stat statbuf;

			if (stat(path.c_str(), &statbuf) == -1) {
				// Doesn't exist. Read another line to complete the path
				if (fgets(mapfile_path, sizeof(mapfile_path), map_fp)) {
					path.append(mapfile_path);

					int32_t eidx = path.size()-1;
					while (isspace(path.at(eidx))) {
						eidx--;
					}

					if (eidx < path.size()-1) {
						path = path.substr(0, eidx+1);
					}
				}
			}

			if (path.find(".so") != std::string::npos) {
				if (so_files.insert(path).second) {
					m_lib_paths.push_back(path);
				}
			}
		}
	}
	fclose(map_fp);

	// Add global scope as an option
	m_search_path.push_back({"<application>", 0});

	m_is_init = true;
	return 0;
}

void *DynSymFinder::findSym(const std::string &sym) {
	std::pair<void *, std::string> ret = findSymLib(sym);
	return ret.first;
}

std::pair<void *,std::string> DynSymFinder::findSymLib(
			const std::string		&sym) {
	std::pair<void *, std::string> ret;

	if (m_last_lib) {
		// Search here first
		void *p = dlsym(m_last_lib, sym.c_str());

		if (p) {
			ret = {p, m_last_lib_path};
		}
	}

	if (!ret.first) {
		// First, work through already-open libraries
		for (auto l_it=m_search_path.begin();
				l_it!=m_search_path.end(); l_it++) {
			void *p;
			if ((p=dlsym(l_it->second, sym.c_str()))) {
				m_last_lib = l_it->second;
				m_last_lib_path = l_it->first;
				ret = {p, m_last_lib_path};
				break;
			}
		}
	}

	if (!ret.first) {
		// Work through the un-loaded libraries
		while (m_lib_paths.size()) {
			std::string path = m_lib_paths.front();
			m_lib_paths.erase(m_lib_paths.begin());
			void *lib = dlopen(path.c_str(), RTLD_LAZY);
			m_search_path.insert(
					m_search_path.begin(),
					{path, lib});
			void *p;

			if ((p=dlsym(lib, sym.c_str()))) {
				m_last_lib = lib;
				m_last_lib_path = path;
				ret = {p, m_last_lib_path};
				break;
			}
		}
	}

	if (ret.first) {
		fprintf(stdout, "Found %s in library %s\n",
				sym.c_str(),
				ret.second.c_str());
	}

	return ret;
	;
}

std::string DynSymFinder::error() {
	return dlerror();
}

} /* namespace tblink_rpc_core */
