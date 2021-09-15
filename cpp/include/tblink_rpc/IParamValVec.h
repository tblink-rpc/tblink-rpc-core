/*
 * IParamValVector.h
 *
 *  Created on: Apr 3, 2021
 *      Author: mballance
 */

#pragma once
#include "IParamVal.h"

namespace tblink_rpc_core {

class IParamValVec;
typedef std::unique_ptr<IParamValVec> IParamValVecUP;
class IParamValVec : public virtual IParamVal {
public:

	virtual ~IParamValVec() { }

	virtual uint32_t size() = 0;

	virtual IParamVal *at(uint32_t idx) = 0;

	template <class T> T *atT(uint32_t idx) {
		return dynamic_cast<T *>(at(idx));
	}

	virtual void push_back(IParamVal *v) = 0;

	virtual IParamValVec *clone() = 0;


};


}

