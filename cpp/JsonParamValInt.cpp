/*
 * ValInt.cpp
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */

#include "JsonParamValInt.h"

#include "nlohmann/json.hpp"

namespace tblink_rpc_core {

JsonParamValInt::JsonParamValInt(int64_t v) :
		JsonParamVal(IParamVal::Int), m_val(v){

}

JsonParamValInt::JsonParamValInt(const nlohmann::json &msg) :
	JsonParamVal(IParamVal::Int) {
	if (msg.is_number()) {
		m_val = msg;
	} else if (msg.is_string()) {
		m_val =std::stoi(msg.get<std::string>());
	} else {
		// ERROR
		m_val = -1;
	}
}

JsonParamValInt::~JsonParamValInt() {
	// TODO Auto-generated destructor stub
}

IParamValInt *JsonParamValInt::clone() {
	return new JsonParamValInt(val_s());
}

nlohmann::json JsonParamValInt::dump() {
	return nlohmann::json(m_val);
}

JsonParamValIntUP JsonParamValInt::mk(const nlohmann::json &msg) {
	if (msg.is_number()) {
		return JsonParamValIntUP(new JsonParamValInt(msg));
	} else if (msg.is_string()) {
		return JsonParamValIntUP(new JsonParamValInt(std::stoi(msg.get<std::string>())));
	} else {
		// ERROR
		return JsonParamValIntUP(new JsonParamValInt(-1));
	}
}

JsonParamValIntUP JsonParamValInt::mk(int32_t v) {
	return JsonParamValIntUP(new JsonParamValInt(v));
}

} /* namespace tblink_rpc_core */
