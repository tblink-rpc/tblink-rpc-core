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
class JsonParamValBool : public JsonParamVal, public virtual IParamValBool {
public:
	JsonParamValBool(bool v=false);

	JsonParamValBool(const nlohmann::json &msg);

	virtual ~JsonParamValBool();

	virtual bool val() override { return m_val; }

	virtual nlohmann::json dump();

	static ValBoolSP mk(bool v=false);

	static ValBoolSP mk(const nlohmann::json &msg);

public:
	static const ValBoolSP	true_v;
	static const ValBoolSP	false_v;

private:
	bool			m_val;
};

} /* namespace tblink_rpc_core */

