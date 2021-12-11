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
#ifdef UNDEFINED
		// Platform-specific discovery...
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
					so_files.insert(path);
				}
			}
		}
		fclose(map_fp);

		// Now, open each library and check for the 'tblink' function
		for (std::unordered_set<std::string>::const_iterator
				it=so_files.begin();
				it!=so_files.end(); it++) {
			void *dlh = dlopen(it->c_str(), RTLD_LAZY);
			if (dlh) {
				void *tblink_h = dlsym(dlh, "tblink");

				if (tblink_h) {
					m_libpath = *it;
				}
				dlclose(dlh);
			}

			if (m_libpath.size() > 0) {
				break;
			}
		}
#endif

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


