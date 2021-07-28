/*
 * JsonParamValFactory.cpp
 *
 *  Created on: Jul 25, 2021
 *      Author: mballance
 */

#include "nlohmann/json.hpp"
#include "JsonParamValBool.h"
#include "JsonParamValFactory.h"
#include "JsonParamValInt.h"
#include "JsonParamValMap.h"
#include "JsonParamValStr.h"
#include "JsonParamValVectorBase.h"

namespace tblink_rpc_core {

JsonParamValFactory::JsonParamValFactory() {
	// TODO Auto-generated constructor stub

}

JsonParamValFactory::~JsonParamValFactory() {
	// TODO Auto-generated destructor stub
}

JsonParamValSP JsonParamValFactory::mk(const nlohmann::json &msg) {
	JsonParamValSP ret;

	switch (msg.type()) {
	case nlohmann::json::value_t::number_integer:
		ret = JsonParamValSP(new JsonParamValInt(msg.get<int64_t>()));
		break;
	case nlohmann::json::value_t::number_unsigned:
		ret = JsonParamValSP(new JsonParamValInt(msg.get<uint64_t>()));
		break;
	case nlohmann::json::value_t::object:
		ret = JsonParamValSP(new JsonParamValMap(msg.get<nlohmann::json>()));
		break;
	case nlohmann::json::value_t::string:
		ret = JsonParamValSP(new JsonParamValStr(msg.get<std::string>()));
		break;
	case nlohmann::json::value_t::array:
		fprintf(stdout, "Type is array: %s\n", msg.dump().c_str());
		ret = JsonParamValSP(new JsonParamValVectorBase(msg));
		break;
	case nlohmann::json::value_t::boolean:
		ret = JsonParamValSP(new JsonParamValBool(msg.get<bool>()));
		break;
	default:
		fprintf(stdout, "Error: unhandled parameter type %s\n", msg.type_name());
	}

	return ret;
}

} /* namespace tblink_rpc_core */
