/*
 * ValVector.h
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */

#pragma once
#include <functional>

#include "tblink_rpc/IParamValVector.h"
#include "JsonParamVal.h"

namespace tblink_rpc_core {

class JsonParamValVectorBase;
typedef std::shared_ptr<JsonParamValVectorBase> JsonParamValVectorBaseSP;
typedef std::unique_ptr<JsonParamValVectorBase> JsonParamValVectorBaseUP;
class JsonParamValVectorBase : public JsonParamVal, public virtual IParamValVector {
public:
	JsonParamValVectorBase();

	JsonParamValVectorBase(
			std::function<JsonParamValSP(const nlohmann::json &)> 	ctor,
			const nlohmann::json 							&msg);

	JsonParamValVectorBase(const nlohmann::json &msg);

	virtual ~JsonParamValVectorBase();

	virtual uint32_t size() override { return m_children.size(); }

	virtual IParamVal *at(uint32_t idx) override { return m_children.at(idx).get(); }

	virtual void push_back(IParamVal *v) override;

	virtual IParamValVector *clone() override;

	const std::vector<JsonParamValUP> &children() const {
		return m_children;
	}

	JsonParamVal *children(uint32_t idx) const {
		return m_children.at(idx).get();
	}

	virtual nlohmann::json dump();

	static JsonParamValVectorBaseUP mk(
			std::function<JsonParamValSP(const nlohmann::json &)> 	ctor,
			const nlohmann::json 							&msg);

	static JsonParamValVectorBaseUP mk();

private:
	std::vector<JsonParamValUP>				m_children;

};


} /* namespace tblink_rpc_core */

