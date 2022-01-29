/*
 * ITbLinkListener.h
 *
 *  Created on: Jan 17, 2022
 *      Author: mballance
 */

#pragma once
#include <memory>
#include "tblink_rpc/ITbLinkEvent.h"

namespace tblink_rpc_core {

class ITbLinkListener;
using ITbLinkListenerUP=std::unique_ptr<ITbLinkListener>;
class ITbLinkListener {
public:

	virtual ~ITbLinkListener() { }

	virtual void notify(const ITbLinkEvent *) = 0;

};

}
