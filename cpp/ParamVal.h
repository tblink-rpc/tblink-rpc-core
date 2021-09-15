/*
 * Val.h
 *
 *  Created on: Sep 30, 2020
 *      Author: ballance
 */

#pragma once
#include <memory>
#include "tblink_rpc/IParamVal.h"

namespace tblink_rpc_core {

class ParamVal;
typedef std::shared_ptr<ParamVal> JsonParamValSP;
typedef std::unique_ptr<ParamVal> ParamValUP;

class ParamVal : public virtual IParamVal {
public:
	ParamVal(IParamVal::TypeE type);

	virtual ~ParamVal();

	virtual IParamVal::TypeE type() override { return m_type; }

private:
	IParamVal::TypeE				m_type;

};


} /* namespace tblink_rpc_core */

