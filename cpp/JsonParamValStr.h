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
class JsonParamValStr : public JsonParamVal, public virtual IParamValStr {
public:
	JsonParamValStr(const std::string &v);

	JsonParamValStr(const nlohmann::json &msg);

	virtual ~JsonParamValStr();

	virtual std::string val() override { return m_val; }

	const std::string &val() const { return m_val; }

	void val(const std::string &v) { m_val = v; }

	virtual nlohmann::json dump() override;

	static JsonParamValStrSP mk(const nlohmann::json &msg);

	static JsonParamValStrSP mk(const std::string &v);

private:
	std::string				m_val;
};

} /* namespace tblink_rpc_core */

