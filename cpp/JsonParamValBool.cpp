/*
 * ValBool.cpp
 *
 *  Created on: Oct 3, 2020
 *      Author: ballance
 */

#include "JsonParamValBool.h"

#include "nlohmann/json.hpp"

namespace tblink_rpc_core {

JsonParamValBool::JsonParamValBool(bool v) :
		JsonParamVal(IParamVal::Bool), m_val(v) {

}

JsonParamValBool::JsonParamValBool(const nlohmann::json &msg) :
	JsonParamVal(IParamVal::Bool) {
	m_val = msg.get<bool>();
}

JsonParamValBool::~JsonParamValBool() {
	// TODO Auto-generated destructor stub
}

IParamValBool *JsonParamValBool::clone() {
	return new JsonParamValBool(val());
}

nlohmann::json JsonParamValBool::dump() {
	return nlohmann::json(m_val);
}

ValBoolUP JsonParamValBool::mk(bool v) {
	return ValBoolUP(new JsonParamValBool(v));
}

ValBoolUP JsonParamValBool::mk(const nlohmann::json &msg) {
	return ValBoolUP(new JsonParamValBool(msg));
}

ValBoolUP JsonParamValBool::true_v() {
	return ValBoolUP(new JsonParamValBool(true));
}

ValBoolUP JsonParamValBool::false_v() {
	return ValBoolUP(new JsonParamValBool(false));
}

} /* namespace tblink_rpc_core */
