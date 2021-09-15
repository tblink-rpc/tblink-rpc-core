/*
 * IParamVal.h
 *
 *  Created on: Apr 3, 2021
 *      Author: mballance
 */

#pragma once
#include <memory>

namespace tblink_rpc_core {

class IParamVal;
typedef std::shared_ptr<IParamVal> IParamValSP;
typedef std::unique_ptr<IParamVal> IParamValUP;
class IParamVal {
public:
	enum TypeE {
		Bool=0,
		Int,
		Map,
		Str,
		Vec
	};

public:

	virtual ~IParamVal() { }

	virtual TypeE type() = 0;

	virtual IParamVal *clone() = 0;

};


}

