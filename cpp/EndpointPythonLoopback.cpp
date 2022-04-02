/*
 * EndpointPythonLoopback.cpp
 *
 *  Created on: Mar 26, 2022
 *      Author: mballance
 */

#include "EndpointPythonLoopback.h"

namespace tblink_rpc_core {

EndpointPythonLoopback::EndpointPythonLoopback(
		const PythonApi 	&api,
		void				*main_t) : m_api(api), m_main_t(main_t) {
	m_interp = 0;
}

EndpointPythonLoopback::~EndpointPythonLoopback() {
	// TODO Auto-generated destructor stub
}

bool EndpointPythonLoopback::init() {
	m_api.PyEval_AcquireThread(m_main_t);

//	m_api.PyEval_AcquireLock(); // Lock the GIL

	if (!(m_interp=m_api.Py_NewInterpreter())) {
		last_error("Failed to create new Python interpreter");
		m_api.PyEval_ReleaseThread(m_interp); // Releases the GIL
		fprintf(stdout, "Failed to create new interpreter\n");
		fflush(stdout);
		return false;
	}

	m_api.PyEval_ReleaseThread(m_interp); // Releases the GIL

	return true;
}

bool EndpointPythonLoopback::launch(
		const std::string 					&main_module,
		const std::vector<std::string>		&args) {

	// Ensure we're in the correct thread
	fprintf(stdout, "Acquire Thread: %p\n", m_interp);
	fflush(stdout);
	m_api.PyEval_AcquireThread(m_interp);

	void *runpy_p = m_api.PyImport_ImportModule("runpy");

	if (!runpy_p) {
		last_error("Failed to load 'runpy' module");
		m_api.PyEval_ReleaseThread(m_interp);
		fprintf(stdout, "Failed to loadl 'runpy'\n");
		fflush(stdout);
		return false;
	}

	void *run_module = m_api.PyObject_GetAttrString(runpy_p, "run_module");

	if (!run_module) {
		last_error("Failed to find 'run_module' in 'runpy'");
		m_api.PyEval_ReleaseThread(m_interp);
		fprintf(stdout, "Failed to load 'run_module'\n");
		fflush(stdout);
		return false;
	}

	void *module_p = m_api.PyImport_ImportModule(main_module);

	if (!module_p) {
		last_error("Failed to load entry module %s", main_module.c_str());

		fprintf(stdout, "Failed to load 'entry' from module %s\n", main_module.c_str());
		fflush(stdout);
		m_api.PyErr_Print();
		fflush(stdout);

		m_api.PyEval_ReleaseThread(m_interp);
		return false;
	}

	void *args_p = m_api.PyTuple_New(args.size());
	for (uint32_t i=0; i<args.size(); i++) {
		m_api.PyTuple_SetItem(args_p, i,
				m_api.PyUnicode_FromString(args.at(i).c_str()));
	}

	void *call_ret = m_api.PyObject_CallObject(run_module, args_p);

	if (!call_ret) {
		last_error("Call failed");

		m_api.PyEval_ReleaseThread(m_interp);
		fprintf(stdout, "Failed to run\n");
		fflush(stdout);
		return false;
	}

	m_api.PyEval_ReleaseThread(m_interp);
	return true;
}

int32_t EndpointPythonLoopback::send_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMap			*params) {
	int32_t ret;

	if (m_primary) {
		m_api.PyEval_AcquireThread(m_interp);
		ret = m_peer->peer_recv_req(method, id, params);
		m_api.PyEval_ReleaseThread(m_interp);
	} else {
		ret = m_peer->peer_recv_req(method, id, params);
	}

	return ret;
}

int32_t EndpointPythonLoopback::send_rsp(
		intptr_t				id,
		IParamValMap			*result,
		IParamValMap			*error) {
	int32_t ret;

	if (m_primary) {
//		m_api.PyEval_AcquireThread(m_interp);
		ret = m_peer->peer_recv_rsp(id, result, error);
//		m_api.PyEval_ReleaseThread(m_interp);
	} else {
		ret = m_peer->peer_recv_rsp(id, result, error);
	}

	return ret;
}


} /* namespace tblink_rpc_core */
