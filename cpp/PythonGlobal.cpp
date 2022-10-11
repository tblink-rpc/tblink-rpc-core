/*
 * PythonGlobal.cpp
 *
 *  Created on: Mar 30, 2022
 *      Author: mballance
 */

#include "PythonGlobal.h"
#include "Debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#else
#include <poll.h>
#include <spawn.h>
#include <unistd.h>
#include <sys/wait.h>
#endif
#include "StringUtil.h"

namespace tblink_rpc_core {

PythonGlobal::PythonGlobal() : m_init(false) {

}

PythonGlobal::~PythonGlobal() {
	// TODO Auto-generated destructor stub
}

bool PythonGlobal::init(const std::string &python_path_hint) {
	bool ret = true;

	// First, determine the actual path to the interpreter
	std::string python_path;
	if (python_path_hint == "") {
		// Need to auto-probe, since the user didn't specify
		const char *tblink_rpc_python = getenv("TBLINK_RPC_PYTHON");

		if (tblink_rpc_python && tblink_rpc_python[0]) {
			python_path = tblink_rpc_python;
		} else {
			python_path = "python3"; // just take the path
		}
	} else {
		python_path = python_path_hint;
	}

	if (m_init) {
		// Check to ensure the same Python has been requested
		// TODO: resolve full path?

		if (python_path != m_python_path) {
			last_error("Attempt to use Python %s after initializing with %s",
					python_path.c_str(), m_python_path.c_str());
			return false;
		}
		m_python_path = python_path;

		// Only need to do the last part once
		return true;
	}

	m_python_path = python_path;

	// Probe Python .dll and internal paths
	std::string pythonpath;
	std::string python_dll;

	if (!get_python_info(python_path, python_dll, pythonpath)) {
		// Assume will leave a note
		return false;
	}

	const char *env_pythonpath = getenv("PYTHONPATH");

	if (env_pythonpath && env_pythonpath[0]) {
		pythonpath = std::string(env_pythonpath) + ":" + pythonpath;
	}

#ifdef _WIN32
	SetEnvironmentVariable((LPCTSTR)"PYTHONPATH", (LPCTSTR)pythonpath.c_str());
#else
	setenv("PYTHONPATH", pythonpath.c_str(), 1);
#endif

	if (!m_api.init(python_dll)) {
		last_error("Failed to load Python API");
		return false;
	}

	m_api.Py_Initialize();

	m_api.PyEval_InitThreads();
	m_main_thread = m_api.PyEval_SaveThread();

	m_init = true;

	return ret;
}

PythonGlobal *PythonGlobal::inst() {
	if (!m_inst.get()) {
		m_inst = std::unique_ptr<PythonGlobal>(new PythonGlobal());
	}
	return m_inst.get();
}

void PythonGlobal::last_error(const char *fmt, ...) {
	char tmp[1024];
	va_list ap;
	va_start(ap, fmt);

	vsnprintf(tmp, sizeof(tmp), fmt, ap);
	m_last_error = tmp;

	va_end(ap);
}

bool PythonGlobal::get_python_info(
			const std::string		&python_path,
			std::string				&python_dll,
			std::string				&pythonpath) {
	// Need to extract:
	// - Python native-library path
	// - Python path elements

	std::vector<std::string> cmdline;
	std::string pycmd;

	pycmd = "import sysconfig,sys; ";
	pycmd += "print('LIBPATH: %s' % str(sysconfig.get_config_vars('LIBDIR','INSTSONAME'))); ";
	pycmd += "print('PYTHONPATH: %s' % str(sys.path))";

	cmdline.push_back(python_path);
	cmdline.push_back("-c");
	cmdline.push_back(pycmd);

#ifdef _WIN32
fprintf(stdout, "Error: Windows not supported\n");
#else
	{
		int cout_pipe[2];

		pipe(cout_pipe);

		posix_spawn_file_actions_t action;
		posix_spawn_file_actions_init(&action);
		posix_spawn_file_actions_addclose(&action, cout_pipe[0]);
		posix_spawn_file_actions_adddup2(&action, cout_pipe[1], 1);
		posix_spawn_file_actions_addclose(&action, cout_pipe[1]);

		char **args = new char *[cmdline.size()+1];

		for (int32_t i=0; i<cmdline.size(); i++) {
			args[i] = strdup(cmdline.at(i).c_str());
		}
		args[cmdline.size()] = 0;

		pid_t pid;
		if (posix_spawnp(&pid, args[0], &action, 0, &args[0], 0) != 0) {
			last_error("failed to start python");
			return false;
		}

		close(cout_pipe[1]);

		// Now, read data from the pipe
		std::string out;
		char buf[1024];
		int sz;

		while ((sz=read(cout_pipe[0], buf, sizeof(buf))) > 0) {
			buf[sz] = 0;
			out += buf;
		}

		std::vector<std::string> lines = StringUtil::split_lines(out);
		for (auto it=lines.begin(); it!=lines.end(); it++) {
			if (it->find("LIBPATH:") == 0) {
				int32_t ci = it->find(':');
				std::vector<std::string> elems = StringUtil::split_pylist(it->substr(ci+1));
				python_dll = elems[0] + "/" + elems[1];
			} else if (it->find("PYTHONPATH:") == 0) {
				int32_t ci = it->find(':');
				std::vector<std::string> elems = StringUtil::split_pylist(it->substr(ci+1));
				for (auto eit=elems.begin(); eit!=elems.end(); eit++) {
					if (*eit != "") {
						if (pythonpath.size() > 0 && pythonpath.at(pythonpath.size()-1) != ':') {
							pythonpath += ":";
						}
						pythonpath += *eit;
					}
				}
			}
		}

		fprintf(stdout, "python_dll: %s\n", python_dll.c_str());
		fprintf(stdout, "pythonpath: %s\n", pythonpath.c_str());

		int32_t exit_code;
		waitpid(pid, &exit_code, 0);
	}
#endif

	return true;
}

std::unique_ptr<PythonGlobal> PythonGlobal::m_inst;

} /* namespace tblink_rpc_core */
