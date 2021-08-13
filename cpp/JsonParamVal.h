/*
 * Val.h
 *
 *  Created on: Sep 30, 2020
 *      Author: ballance
 */

#pragma once
#include <memory>
#include "nlohmann/json_fwd.hpp"
#include "tblink_rpc/IParamVal.h"

namespace tblink_rpc_core {

class JsonParamVal;
typedef std::shared_ptr<JsonParamVal> JsonParamValSP;
typedef std::unique_ptr<JsonParamVal> JsonParamValUP;

class JsonParamVal : public virtual IParamVal {
public:
	JsonParamVal(IParamVal::TypeE type);

	virtual ~JsonParamVal();

	virtual IParamVal::TypeE type() override { return m_type; }

	virtual nlohmann::json dump();

	static JsonParamValUP load(const nlohmann::json &msg);

private:
	IParamVal::TypeE				m_type;

};


} /* namespace tblink_rpc_core */

