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
class IParamValVector : public IParamVal {
public:

	virtual ~IParamValVector() { }

	virtual uint32_t size() = 0;

	virtual IParamValSP at(uint32_t idx) = 0;

	template <class T> std::shared_ptr<T> atT(uint32_t idx) {
		return std::dynamic_pointer_cast<T>(at(idx));
	}

	virtual void push_back(IParamValSP v) = 0;


};


}

