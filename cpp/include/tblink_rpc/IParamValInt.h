/*
 * IParamValInt.h
 *
 *  Created on: Apr 3, 2021
 *      Author: mballance
 */

#pragma once
#include "IParamVal.h"

namespace tblink_rpc_core {

class IParamValInt;
typedef std::shared_ptr<IParamValInt> IParamValIntSP;
typedef std::unique_ptr<IParamValInt> IParamValIntUP;
class IParamValInt : public virtual IParamVal {
public:

	virtual ~IParamValInt() { }

	virtual uint64_t val_u() = 0;

	virtual int64_t val_s() = 0;

	virtual IParamValInt *clone() = 0;

};

}

