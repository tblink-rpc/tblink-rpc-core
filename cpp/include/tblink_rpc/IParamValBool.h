/*
 * IParamValInt.h
 *
 *  Created on: Apr 3, 2021
 *      Author: mballance
 */

#pragma once
#include "IParamVal.h"

namespace tblink_rpc_core {

class IParamValBool;
typedef std::shared_ptr<IParamValBool> IParamValBoolSP;
class IParamValBool : public virtual IParamVal {
public:

	virtual ~IParamValBool() { }

	virtual bool val() = 0;

};

}

