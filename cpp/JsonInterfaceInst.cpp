/*
 * JsonInterfaceInst.cpp
 *
 *  Created on: Jul 13, 2021
 *      Author: mballance
 */

#include "JsonInterfaceInst.h"

namespace tblink_rpc_core {

JsonInterfaceInst::JsonInterfaceInst(
		IInterfaceType			*type,
		const std::string		&inst_name,
		const invoke_req_f		&req_f) {
	m_type = type;
	m_inst_name = inst_name;
	m_req_f = req_f;

}

JsonInterfaceInst::~JsonInterfaceInst() {
	// TODO Auto-generated destructor stub
}

int32_t JsonInterfaceInst::invoke(
		IMethodType									*method,
		IParamValVectorSP							params,
		const invoke_rsp_f							&completion_f) {

}

IParamValSP JsonInterfaceInst::invoke_nb(
		IMethodType									*method,
		IParamValVectorSP							params) {

}

void JsonInterfaceInst::respond(
		intptr_t									call_id,
		IParamValSP									ret) {

}


} /* namespace tblink_rpc_core */
