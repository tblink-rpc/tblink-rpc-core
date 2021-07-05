/*
 * IParamValFactory.h
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */

#pragma once

#include "tblink_rpc/IParamValBool.h"
#include "tblink_rpc/IParamValInt.h"
#include "tblink_rpc/IParamValMap.h"
#include "tblink_rpc/IParamValStr.h"
#include "tblink_rpc/IParamValVector.h"

namespace tblink_rpc_core {

class IParamValFactory {
public:

	virtual ~IParamValFactory() { }

	virtual IParamValBoolSP mkValBool(bool val) = 0;

	virtual IParamValIntSP mkValIntU(uint64_t val) = 0;

	virtual IParamValIntSP mkValIntS(int64_t val) = 0;

	virtual IParamValMapSP mkValMap() = 0;

	virtual IParamValStrSP mkValStr(const std::string &val) = 0;

	virtual IParamValVectorSP mkVector() = 0;


};

}

