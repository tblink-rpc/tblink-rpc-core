/*
 * JsonParamValMap.h
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */
#pragma once
#include "tblink_rpc/IParamValMap.h"
#include "JsonParamVal.h"


namespace tblink_rpc_core {

class JsonParamValMap;
typedef std::shared_ptr<JsonParamValMap> JsonParamValMapSP;
typedef std::unique_ptr<JsonParamValMap> JsonParamValMapUP;
class JsonParamValMap : public JsonParamVal, public virtual IParamValMap {
public:
	JsonParamValMap();

	JsonParamValMap(const nlohmann::json &msg);

	virtual ~JsonParamValMap();

	virtual const std::set<std::string> &keys() override;

	virtual bool hasKey(const std::string &key) override;

	virtual IParamVal *getVal(
			const std::string		&key) override;

	virtual void setVal(
			const std::string		&key,
			IParamVal				*val) override;

	virtual IParamValMap *clone() override;

	virtual nlohmann::json dump();

	static JsonParamValMapUP mk();

	static JsonParamValMapUP mk(const nlohmann::json &msg);

private:
	std::map<std::string, JsonParamValUP>	m_map;
	std::set<std::string>					m_keys;

};

} /* namespace tblink_rpc_core */

