/*
 * ValInt.cpp
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */

#include "ParamValInt.h"

namespace tblink_rpc_core {

ParamValInt::ParamValInt(int64_t v) :
		ParamVal(IParamVal::Int), m_val(v){

}

ParamValInt::~ParamValInt() {
	// TODO Auto-generated destructor stub
}

IParamValInt *ParamValInt::clone() {
	return new ParamValInt(val_s());
}

ParamValIntUP ParamValInt::mk(int32_t v) {
	return ParamValIntUP(new ParamValInt(v));
}

} /* namespace tblink_rpc_core */
