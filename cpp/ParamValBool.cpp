/*
 * ValBool.cpp
 *
 *  Created on: Oct 3, 2020
 *      Author: ballance
 */

#include "ParamValBool.h"

namespace tblink_rpc_core {

ParamValBool::ParamValBool(bool v) :
		ParamVal(IParamVal::Bool), m_val(v) {

}

ParamValBool::~ParamValBool() {
	// TODO Auto-generated destructor stub
}

IParamValBool *ParamValBool::clone() {
	return new ParamValBool(val());
}

ParamValBoolUP ParamValBool::mk(bool v) {
	return ParamValBoolUP(new ParamValBool(v));
}

ParamValBoolUP ParamValBool::true_v() {
	return ParamValBoolUP(new ParamValBool(true));
}

ParamValBoolUP ParamValBool::false_v() {
	return ParamValBoolUP(new ParamValBool(false));
}

} /* namespace tblink_rpc_core */
