/*
 * ValBool.h
 *
 *  Created on: Oct 3, 2020
 *      Author: ballance
 */

#pragma once
#include "ParamVal.h"
#include "tblink_rpc/IParamValBool.h"

namespace tblink_rpc_core {

class ParamValBool;
typedef std::unique_ptr<ParamValBool> ParamValBoolUP;
class ParamValBool : public ParamVal, public virtual IParamValBool {
public:
	ParamValBool(bool v=false);

	virtual ~ParamValBool();

	virtual bool val() override { return m_val; }

	virtual IParamValBool *clone() override;

	static ParamValBoolUP mk(bool v=false);

	static ParamValBoolUP true_v();

	static ParamValBoolUP false_v();

private:
	bool			m_val;
};

} /* namespace tblink_rpc_core */

