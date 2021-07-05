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
class JsonParamValMap : public JsonParamVal, public virtual IParamValMap {
public:
	JsonParamValMap();

	virtual ~JsonParamValMap();

	virtual const std::set<std::string> &keys() override;

	virtual bool hasKey(const std::string &key) override;

	virtual IParamValSP getVal(
			const std::string		&key) override;

	virtual void setVal(
			const std::string		&key,
			IParamValSP				val) override;

	static JsonParamValMapSP mk();

private:
	std::map<std::string, IParamValSP>		m_map;
	std::set<std::string>					m_keys;

};

} /* namespace tblink_rpc_core */

