/*
 * TbLinkListenerClosure.h
 *
 *  Created on: Jan 18, 2022
 *      Author: mballance
 */
#pragma once
#include <Python.h>
#include "tblink_rpc/ITbLinkListener.h"


class TbLinkListenerClosure : public tblink_rpc_core::ITbLinkListener {
public:
	TbLinkListenerClosure(PyObject *hndl);

	virtual ~TbLinkListenerClosure();

	virtual void notify(const tblink_rpc_core::ITbLinkEvent *) override;

private:
	PyObject					*m_hndl;

};

