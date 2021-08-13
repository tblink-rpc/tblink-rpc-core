/*
 * ValBool.h
 *
 *  Created on: Oct 3, 2020
 *      Author: ballance
 */

#pragma once
#include "tblink_rpc/IParamValBool.h"
#include "JsonParamVal.h"

namespace tblink_rpc_core {

class JsonParamValBool;
typedef std::shared_ptr<JsonParamValBool> ValBoolSP;
typedef std::unique_ptr<JsonParamValBool> ValBoolUP;
class JsonParamValBool : public JsonParamVal, public virtual IParamValBool {
public:
	JsonParamValBool(bool v=false);

	JsonParamValBool(const nlohmann::json &msg);

	virtual ~JsonParamValBool();

	virtual bool val() override { return m_val; }

	virtual IParamValBool *clone() override;

	virtual nlohmann::json dump();

	static ValBoolUP mk(bool v=false);

	static ValBoolUP mk(const nlohmann::json &msg);

	static ValBoolUP true_v();

	static ValBoolUP false_v();

private:
	bool			m_val;
};

} /* namespace tblink_rpc_core */

