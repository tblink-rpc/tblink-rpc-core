/*
 * ValVector.cpp
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */

#include "ParamValVec.h"
#include "JsonParamValFactory.h"

namespace tblink_rpc_core {

ParamValVec::ParamValVec() :
	ParamVal(IParamVal::Vec) {

}

ParamValVec::~ParamValVec() {
	// TODO Auto-generated destructor stub
}

void ParamValVec::push_back(IParamVal *v) {
	m_children.push_back(ParamValUP(dynamic_cast<ParamVal *>(v)));
}

IParamValVec *ParamValVec::clone() {
	ParamValVec *ret = new ParamValVec();

	for (std::vector<ParamValUP>::const_iterator
			it=m_children.begin();
			it!=m_children.end(); it++) {
		ret->push_back((*it)->clone());
	}
	return ret;
}

ParamValVecUP ParamValVec::mk() {
	return ParamValVecUP(new ParamValVec());
}

} /* namespace tblink_rpc_core */
