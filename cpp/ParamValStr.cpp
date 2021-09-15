/*
 * ValStr.cpp
 *
 *  Created on: Sep 30, 2020
 *      Author: ballance
 */

#include "ParamValStr.h"

namespace tblink_rpc_core {

ParamValStr::ParamValStr(const std::string &v) :
		ParamVal(IParamVal::Str), m_val(v) {

}

ParamValStr::~ParamValStr() {
	// TODO Auto-generated destructor stub
}

IParamValStr *ParamValStr::clone() {
	return new ParamValStr(val());
}

ParamValStrUP ParamValStr::mk(const std::string &v) {
	return ParamValStrUP(new ParamValStr(v));
}

} /* namespace tblink_rpc_core */
