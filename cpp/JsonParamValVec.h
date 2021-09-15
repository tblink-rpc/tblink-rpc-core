/*
 * ValVector.h
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */

#pragma once
#include <functional>

#include "include/tblink_rpc/IParamValVec.h"
#include "JsonParamVal.h"

namespace tblink_rpc_core {

class JsonParamValVec;
typedef std::shared_ptr<JsonParamValVec> JsonParamValVectorBaseSP;
typedef std::unique_ptr<JsonParamValVec> JsonParamValVectorBaseUP;
class JsonParamValVec : public JsonParamVal, public virtual IParamValVec {
public:
	JsonParamValVec();

	JsonParamValVec(
			std::function<JsonParamValSP(const nlohmann::json &)> 	ctor,
			const nlohmann::json 							&msg);

	JsonParamValVec(const nlohmann::json &msg);

	virtual ~JsonParamValVec();

	virtual uint32_t size() override { return m_children.size(); }

	virtual IParamVal *at(uint32_t idx) override { return m_children.at(idx).get(); }

	virtual void push_back(IParamVal *v) override;

	virtual IParamValVec *clone() override;

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

