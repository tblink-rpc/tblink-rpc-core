/*
 * JsonParamValFactory.h
 *
 *  Created on: Jul 25, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IParamValInt.h"
#include "tblink_rpc/IParamValMap.h"
#include "tblink_rpc/IParamValStr.h"
#include "tblink_rpc/IParamValVector.h"
#include "nlohmann/json_fwd.hpp"
#include "JsonParamVal.h"

namespace tblink_rpc_core {

class JsonParamValFactory {
public:
	JsonParamValFactory();

	virtual ~JsonParamValFactory();

	static JsonParamValUP mk(const nlohmann::json &msg);

};

} /* namespace tblink_rpc_core */

