/*
 * PythonGlobal.h
 *
 *  Created on: Mar 30, 2022
 *      Author: mballance
 */

#pragma once
#include <memory>
#include <string>
#include "PythonApi.h"

namespace tblink_rpc_core {

class PythonGlobal {
public:
	PythonGlobal();

	virtual ~PythonGlobal();

	const PythonApi &api() const { return m_api; }

	bool init(const std::string &python_path_hint);

	void *main_thread() const { return m_main_thread; }

	const std::string &last_error() const { return m_last_error; }

	static PythonGlobal *inst();

private:

	void last_error(const char *fmt, ...);

	bool get_python_info(
			const std::string 	&python_path,
			std::string			&python_dll,
			std::string			&pythonpath);

private:
	bool										m_init;
	std::string									m_python_path;
	PythonApi									m_api;
	std::string									m_last_error;
	void										*m_main_thread;
	static std::unique_ptr<PythonGlobal>		m_inst;
};

} /* namespace tblink_rpc_core */

