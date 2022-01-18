/*
 * ITbLinkEvent.h
 *
 *  Created on: Jan 17, 2022
 *      Author: mballance
 */

#pragma once

namespace tblink_rpc_core {

enum class TbLinkEventKind {
	AddEndpoint,
	RemEndpoint
};

class ITbLinkEvent {
public:

	virtual ~ITbLinkEvent() { }

	virtual TbLinkEventKind kind() = 0;

	virtual void *hndl() = 0;

	template <class T> T *hndlT() {
		return dynamic_cast<T *>(hndl());
	}


};

}
