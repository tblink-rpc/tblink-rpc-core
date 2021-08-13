/*
 * ValStr.cpp
 *
 *  Created on: Sep 30, 2020
 *      Author: ballance
 */

#include "JsonParamValStr.h"

#include "nlohmann/json.hpp"

namespace tblink_rpc_core {

JsonParamValStr::JsonParamValStr(const nlohmann::json &msg) :
	JsonParamVal(IParamVal::Str) {
	m_val = msg.get<std::string>();
}

JsonParamValStr::JsonParamValStr(const std::string &v) :
		JsonParamVal(IParamVal::Str), m_val(v) {

}

JsonParamValStr::~JsonParamValStr() {
	// TODO Auto-generated destructor stub
}

IParamValStr *JsonParamValStr::clone() {
	return new JsonParamValStr(val());
}

nlohmann::json JsonParamValStr::dump() {
	return nlohmann::json(m_val);
}

JsonParamValStrUP JsonParamValStr::mk(const nlohmann::json &msg) {
	return JsonParamValStrUP(new JsonParamValStr(msg));
}

JsonParamValStrUP JsonParamValStr::mk(const std::string &v) {
	return JsonParamValStrUP(new JsonParamValStr(v));
}

} /* namespace tblink_rpc_core */
