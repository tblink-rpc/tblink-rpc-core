/*
 * BfmReqRsp.cpp
 *
 *  Created on: Sep 15, 2021
 *      Author: mballance
 */

#include "BfmReqRsp.h"
using namespace tblink_rpc_core;

BfmReqRsp::BfmReqRsp() : m_ifinst(0), m_req_mt(0), m_rsp_mt(0) {
	// TODO Auto-generated constructor stub

}

BfmReqRsp::~BfmReqRsp() {
	// TODO Auto-generated destructor stub
}

// Import
void BfmReqRsp::req(int32_t v) {
	if (m_ifinst->is_mirror()) {
		// Really a call-in, since we're a mirror
	} else {
		// TODO: pack params and invoke
		IParamValVec *params = m_ifinst->mkValVec();
		params->push_back(m_ifinst->mkValIntS(v, 32));

		IParamValUP ret(m_ifinst->invoke(
				m_rsp_mt,
				params));
	}
}

// Export
void BfmReqRsp::rsp(int32_t v) {
	if (m_ifinst->is_mirror()) {
		// Really a call-out, since we're a mirror
		// TODO: pack params and invoke
		IParamValVec *params = m_ifinst->mkValVec();
		params->push_back(m_ifinst->mkValIntS(v, 32));

		IParamValUP ret(m_ifinst->invoke(
				m_rsp_mt,
				params));
	} else {
		// Really a call-in, since we're not a mirror
	}
}

IInterfaceType *BfmReqRsp::registerType(tblink_rpc_core::IEndpoint *ep) {
	IInterfaceType *iftype = ep->findInterfaceType("BfmReqRsp");
	if (iftype) {
		return iftype;
	}

	IInterfaceTypeBuilder *iftype_b = ep->newInterfaceTypeBuilder("BfmReqRsp");
	IMethodTypeBuilder *mtb;

	mtb = iftype_b->newMethodTypeBuilder(
			"req",
			0,
			0,
			false,
			false);
	fprintf(stdout, "mtb: %p\n", mtb);
	mtb->add_param(
			"v",
			iftype_b->mkTypeInt(true, 32));
	iftype_b->add_method(mtb);

	mtb = iftype_b->newMethodTypeBuilder(
			"rsp",
			1,
			0,
			true,
			false);
	iftype_b->add_method(mtb);

	iftype = ep->defineInterfaceType(iftype_b);


	return iftype;
}

BfmReqRsp *BfmReqRsp::mkInst(
			tblink_rpc_core::IEndpoint 	*ep,
			const std::string			&inst_name,
			bool						is_mirror) {
	BfmReqRsp *ret = new BfmReqRsp();
	IInterfaceType *iftype = registerType(ep);

	ret->m_req_mt = iftype->findMethod("req");
	ret->m_rsp_mt = iftype->findMethod("rsp");

	ret->m_ifinst = ep->defineInterfaceInst(
			iftype,
			inst_name,
			is_mirror,
			std::bind(&BfmReqRsp::invoke,
					ret,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					std::placeholders::_4));

	return ret;
}

void BfmReqRsp::invoke(
		IInterfaceInst				*ifinst,
		IMethodType					*method_t,
		intptr_t					call_id,
		IParamValVec				*params) {
	switch (method_t->id()) {
	case 0: {
		req(params->atT<IParamValInt>(0)->val_s());
		ifinst->invoke_rsp(call_id, 0);
	} break;
	case 1: {
		rsp(params->atT<IParamValInt>(0)->val_s());
		ifinst->invoke_rsp(call_id, 0);
	} break;
	}
}

