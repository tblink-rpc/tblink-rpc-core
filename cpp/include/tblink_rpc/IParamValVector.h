/*
 * IParamValVector.h
 *
 *  Created on: Apr 3, 2021
 *      Author: mballance
 */

#pragma once
#include "IParamVal.h"

namespace tblink_rpc_core {

class IParamValVector;
typedef std::shared_ptr<IParamValVector> IParamValVectorSP;
typedef std::unique_ptr<IParamValVector> IParamValVectorUP;
class IParamValVector : public virtual IParamVal {
public:

	virtual ~IParamValVector() { }

	virtual uint32_t size() = 0;

	virtual IParamVal *at(uint32_t idx) = 0;

	template <class T> T *atT(uint32_t idx) {
		return dynamic_cast<T *>(at(idx));
	}

	virtual void push_back(IParamVal *v) = 0;

	virtual IParamValVector *clone() = 0;


};


}

