/*
 * JsonParamValMap.h
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */
#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "ParamVal.h"
#include "tblink_rpc/IParamValMap.h"


namespace tblink_rpc_core {

class ParamValMap;
typedef std::unique_ptr<ParamValMap> ParamValMapUP;
class ParamValMap : public ParamVal, public virtual IParamValMap {
public:
	ParamValMap();

	virtual ~ParamValMap();

	virtual const std::set<std::string> &keys() override;

	virtual bool hasKey(const std::string &key) override;

	virtual IParamVal *getVal(
			const std::string		&key) override;

	virtual void setVal(
			const std::string		&key,
			IParamVal				*val) override;

	virtual IParamValMap *clone() override;

	static ParamValMapUP mk();

private:
	std::unordered_map<std::string, ParamValUP>		m_map;
	std::set<std::string>							m_keys;

};

} /* namespace tblink_rpc_core */

