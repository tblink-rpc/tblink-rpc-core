/*
 * JsonParamValMap.cpp
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */

#include "nlohmann/json.hpp"
#include "JsonParamValFactory.h"
#include "JsonParamValInt.h"
#include "JsonParamValMap.h"
#include "JsonParamValStr.h"

namespace tblink_rpc_core {

JsonParamValMap::JsonParamValMap() :
	JsonParamVal(IParamVal::Map) {
	// TODO Auto-generated constructor stub

}

JsonParamValMap::JsonParamValMap(const nlohmann::json &msg) :
	JsonParamVal(IParamVal::Map) {
	for (nlohmann::json::const_iterator
			it=msg.begin();
			it!=msg.end(); it++) {
		setVal(it.key(), JsonParamValFactory::mk(it.value()).release());
	}
}

JsonParamValMap::~JsonParamValMap() {
	// TODO Auto-generated destructor stub
}

const std::set<std::string> &JsonParamValMap::keys() {
	return m_keys;
}

bool JsonParamValMap::hasKey(const std::string &key) {
	return (m_map.find(key) != m_map.end());
}

IParamVal *JsonParamValMap::getVal(
		const std::string		&key) {
	std::map<std::string,JsonParamValUP>::const_iterator it;

	if ((it=m_map.find(key)) != m_map.end()) {
		return it->second.get();
	} else {
		return 0;
	}
}

void JsonParamValMap::setVal(
		const std::string		&key,
		IParamVal				*val) {
	std::map<std::string,JsonParamValUP>::const_iterator it;
	JsonParamVal *jval = dynamic_cast<JsonParamVal *>(val);
	m_keys.insert(key);

	if ((it=m_map.find(key)) != m_map.end()) {
		m_map.erase(it);
	}
	m_map.insert({key, JsonParamValUP(jval)});
}

IParamValMap *JsonParamValMap::clone() {
	JsonParamValMap *ret = new JsonParamValMap();

	for (std::map<std::string,JsonParamValUP>::const_iterator
			it=m_map.begin();
			it!=m_map.end(); it++) {
		ret->setVal(it->first, it->second->clone());
	}

	return ret;
}

nlohmann::json JsonParamValMap::dump() {
	nlohmann::json ret;

	if (m_map.size() == 0) {
		ret = nlohmann::json::object();
	} else {
		for (std::map<std::string,JsonParamValUP>::const_iterator
				it=m_map.begin();
				it!=m_map.end(); it++) {
			ret[it->first] = it->second->dump();
		}
	}

	return ret;
}

JsonParamValMapUP JsonParamValMap::mk() {
	return JsonParamValMapUP(new JsonParamValMap());
}

JsonParamValMapUP JsonParamValMap::mk(const nlohmann::json &msg) {
	return JsonParamValMapUP(new JsonParamValMap(msg));
}

} /* namespace tblink_rpc_core */
