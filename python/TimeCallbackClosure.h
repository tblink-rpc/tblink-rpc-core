/*
 * TimeCallbackClosure.h
 *
 *  Created on: Jan 22, 2022
 *      Author: mballance
 */

#pragma once
#include <Python.h>
#include <functional>

class TimeCallbackClosure {
public:
	TimeCallbackClosure(PyObject *obj);

	virtual ~TimeCallbackClosure();

	std::function<void()> closure();

private:
	void callback();

private:
	PyObject					*m_obj;
};

