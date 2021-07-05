/*
 * ValInt.h
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */

#pragma once
#include "tblink_rpc/IParamValInt.h"
#include "JsonParamVal.h"

namespace tblink_rpc_core {

class JsonParamValInt;
typedef std::shared_ptr<JsonParamValInt> JsonParamValIntSP;
class JsonParamValInt : public JsonParamVal, public virtual IParamValInt {
public:
	JsonParamValInt(int64_t v);

	JsonParamValInt(const nlohmann::json &msg);

	virtual ~JsonParamValInt();

	int64_t val() const { return m_val; }

	void val(int64_t v) { m_val = v; }

	virtual uint64_t val_u() override { return m_val; }

	virtual int64_t val_s() override { return m_val; }

	virtual nlohmann::json dump() override;

	static JsonParamValIntSP mk(const nlohmann::json &msg);

	static JsonParamValIntSP mk(int32_t v);

private:
	int64_t					m_val;

};

} /* namespace tblink_rpc_core */

