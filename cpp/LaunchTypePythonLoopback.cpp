/*
 * LaunchTypePythonLoopback.cpp
 *
 *  Created on: Dec 29, 2021
 *      Author: mballance
 */

#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <poll.h>
#include <unistd.h>
#else
#include <windows.h>
#endif
#include <sys/wait.h>
#include "Debug.h"
#include "LaunchParams.h"
#include "LaunchTypePythonLoopback.h"
#include "LaunchTypeRegistration.h"
#include "EndpointLoopback.h"
#include "PythonApi.h"
#include "StringUtil.h"

#define EN_DEBUG_LAUNCH_TYPE_PYTHON_LOOPBACK

#ifdef EN_DEBUG_LAUNCH_TYPE_PYTHON_LOOPBACK
#define DEBUG_ENTER(fmt, ...) DEBUG_ENTER_BASE(LaunchTypePythonLoopback, fmt, ##__VA_ARGS__)
#define DEBUG_LEAVE(fmt, ...) DEBUG_LEAVE_BASE(LaunchTypePythonLoopback, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) DEBUG_BASE(LaunchTypePythonLoopback, fmt, ##__VA_ARGS__)
#else
#define DEBUG_ENTER(fmt, ...)
#define DEBUG_LEAVE(fmt, ...)
#define DEBUG(fmt, ...)
#endif

namespace tblink_rpc_core {

LaunchTypePythonLoopback::LaunchTypePythonLoopback() {
	// TODO Auto-generated constructor stub

}

LaunchTypePythonLoopback::~LaunchTypePythonLoopback() {
	// TODO Auto-generated destructor stub
}

ILaunchType::result_t LaunchTypePythonLoopback::launch(
		ILaunchParams 		*params,
		IEndpointServices	*services) {
	DEBUG_ENTER("launch");
	ITbLink *tblink = TbLink::inst();

	if (!params->has_param("module")) {
		return {0, "No 'module' parameter specified"};
	}

	std::string python_path = "python3";
	if (params->has_param("python")	) {
		python_path = params->get_param("python");
	}  // TODO: honor environment variable

	std::string python_dll;
	std::string pythonpath;
	if (get_python_info(python_path, python_dll, pythonpath) != 0) {
		return {0, "Failed to probe Python interpreter"};
	}

	const char *env_pythonpath = getenv("PYTHONPATH");

	if (env_pythonpath && env_pythonpath[0]) {
		// Append
		pythonpath = std::string(env_pythonpath) + ":" + pythonpath;
	}

	fprintf(stdout, "Final pythonpath: %s\n", pythonpath.c_str());
	setenv("PYTHONPATH", pythonpath.c_str(), 1);



	PythonApi pyapi;

	if (pyapi.init(python_dll) != 0) {
		return {0, "Failed to load the Python API"};
	}

	pyapi.Py_Initialize();
	void *runpy_p = pyapi.PyImport_ImportModule("runpy");
	/*
	void *sys_p = pyapi.PyImport_ImportModule("sys");

	if (!sys_p) {
		return {0, "Failed to load 'sys'"};
	}
	 */

	void *module_p = pyapi.PyImport_ImportModule(params->get_param("module"));

	if (!module_p) {
		char tmp[256];
		sprintf(tmp, "Failed to load entry module %s", params->get_param("module").c_str());
		pyapi.PyErr_Print();
		return {0, tmp};
	}

	void *run_module = pyapi.PyObject_GetAttrString(runpy_p, "run_module");

	if (!run_module) {
		return {0, "Failed to find run_module"};
	}

	void *args = pyapi.PyTuple_New(1);
	pyapi.PyTuple_SetItem(args, 0, pyapi.PyUnicode_FromString(
			params->get_param("module")));

	// Setup the endpoint
	EndpointLoopback *ep = new EndpointLoopback();

	tblink->addEndpoint(ep);
	tblink->addEndpoint(ep->peer());

	void *call_ret = pyapi.PyObject_CallObject(run_module, args);

	if (!call_ret) {
		fprintf(stdout, "Error: Call failed\n");
	}

	DEBUG_LEAVE("launch");
	return {ep, "Launch failure"};
}

/**
 *
 */
ILaunchParams *LaunchTypePythonLoopback::newLaunchParams() {
	return new LaunchParams();
}

int32_t LaunchTypePythonLoopback::get_python_info(
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
			fprintf(stdout, "failed to start python\n");
			return -1;
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

	return 0;
}

static LaunchTypeRegistration<LaunchTypePythonLoopback> m_registration;

} /* namespace tblink_rpc_core */
