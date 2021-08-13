/*
 * Val.cpp
 *
 *  Created on: Sep 30, 2020
 *      Author: ballance
 */

#include "JsonParamVal.h"

#include "nlohmann/json.hpp"

namespace tblink_rpc_core {

JsonParamVal::JsonParamVal(IParamVal::TypeE type) : m_type(type) {
	// TODO Auto-generated constructor stub

}

JsonParamVal::~JsonParamVal() {
	// TODO Auto-generated destructor stub
}

nlohmann::json JsonParamVal::dump() {
	return nlohmann::json();
}

} /* namespace tblink_rpc_core */
