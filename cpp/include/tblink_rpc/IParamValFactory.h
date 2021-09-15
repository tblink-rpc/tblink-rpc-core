/*
 * IParamValFactory.h
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */

#pragma once

#include "IParamValVec.h"
#include "tblink_rpc/IParamValBool.h"
#include "tblink_rpc/IParamValInt.h"
#include "tblink_rpc/IParamValMap.h"
#include "tblink_rpc/IParamValStr.h"

namespace tblink_rpc_core {

class IParamValFactory {
public:

	virtual ~IParamValFactory() { }

	virtual IParamValBool *mkValBool(bool val) = 0;

	virtual IParamValInt *mkValIntU(uint64_t val) = 0;

	virtual IParamValInt *mkValIntS(int64_t val) = 0;

	virtual IParamValMap *mkValMap() = 0;

	virtual IParamValStr *mkValStr(const std::string &val) = 0;

	virtual IParamValVec *mkValVec() = 0;


};

}

