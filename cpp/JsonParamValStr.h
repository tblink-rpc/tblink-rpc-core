/*
 * ValStr.h
 *
 *  Created on: Sep 30, 2020
 *      Author: ballance
 */

#pragma once
#include <string>

#include "tblink_rpc/IParamValStr.h"
#include "JsonParamVal.h"

namespace tblink_rpc_core {

class JsonParamValStr;
typedef std::shared_ptr<JsonParamValStr> JsonParamValStrSP;
typedef std::unique_ptr<JsonParamValStr> JsonParamValStrUP;
class JsonParamValStr : public JsonParamVal, public virtual IParamValStr {
public:
	JsonParamValStr(const std::string &v);

	JsonParamValStr(const nlohmann::json &msg);

	virtual ~JsonParamValStr();

	virtual std::string val() override { return m_val; }

	const std::string &val() const { return m_val; }

	void val(const std::string &v) { m_val = v; }

	virtual IParamValStr *clone() override;

	virtual nlohmann::json dump() override;

	static JsonParamValStrUP mk(const nlohmann::json &msg);

	static JsonParamValStrUP mk(const std::string &v);

private:
	std::string				m_val;
};

} /* namespace tblink_rpc_core */

