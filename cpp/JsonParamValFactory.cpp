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

JsonParamValUP JsonParamValFactory::mk(const nlohmann::json &msg) {
	JsonParamVal *ret = 0;

	switch (msg.type()) {
	case nlohmann::json::value_t::number_integer:
		ret = new JsonParamValInt(msg.get<int64_t>());
		break;
	case nlohmann::json::value_t::number_unsigned:
		ret = new JsonParamValInt(msg.get<uint64_t>());
		break;
	case nlohmann::json::value_t::object:
		ret = new JsonParamValMap(msg.get<nlohmann::json>());
		break;
	case nlohmann::json::value_t::string:
		ret = new JsonParamValStr(msg.get<std::string>());
		break;
	case nlohmann::json::value_t::array:
		fprintf(stdout, "Type is array: %s\n", msg.dump().c_str());
		ret = new JsonParamValVectorBase(msg);
		break;
	case nlohmann::json::value_t::boolean:
		ret = new JsonParamValBool(msg.get<bool>());
		break;
	default:
		fprintf(stdout, "Error: unhandled parameter type %s\n", msg.type_name());
	}

	return JsonParamValUP(ret);
}

} /* namespace tblink_rpc_core */
