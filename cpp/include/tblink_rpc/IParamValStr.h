/*
 * IParamValStr.h
 *
 *  Created on: Apr 3, 2021
 *      Author: mballance
 */

#pragma once
#include "IParamVal.h"

namespace tblink_rpc_core {

class IParamValStr;
typedef std::shared_ptr<IParamValStr> IParamValStrSP;
typedef std::unique_ptr<IParamValStr> IParamValStrUP;
class IParamValStr : public virtual IParamVal {
public:
	virtual ~IParamValStr() { }

	virtual std::string val() = 0;

	virtual IParamValStr *clone() = 0;

};

}
