/*
 * JsonParamValFactory.h
 *
 *  Created on: Jul 25, 2021
 *      Author: mballance
 */

#pragma once
#include "include/tblink_rpc/IParamValVec.h"
#include "tblink_rpc/IParamValInt.h"
#include "tblink_rpc/IParamValMap.h"
#include "tblink_rpc/IParamValStr.h"
#include "nlohmann/json_fwd.hpp"
#include "ParamVal.h"

namespace tblink_rpc_core {

class JsonParamValFactory {
public:
	JsonParamValFactory();

	virtual ~JsonParamValFactory();

	IParamVal *mk(const nlohmann::json &msg);

	template <class T> T *mkT(const nlohmann::json &msg) {
		return dynamic_cast<T *>(mk(msg));
	}

	nlohmann::json mk(IParamVal *val);

};

} /* namespace tblink_rpc_core */

