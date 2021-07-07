/*
 * JsonParamValMap.cpp
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */

#include "JsonParamValMap.h"
#include "nlohmann/json.hpp"

namespace tblink_rpc_core {

JsonParamValMap::JsonParamValMap() {
	// TODO Auto-generated constructor stub

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

IParamValSP JsonParamValMap::getVal(
		const std::string		&key) {
	std::map<std::string,JsonParamValSP>::const_iterator it;

	if ((it=m_map.find(key)) != m_map.end()) {
		return it->second;
	} else {
		return 0;
	}
}

void JsonParamValMap::setVal(
		const std::string		&key,
		IParamValSP				val) {
	std::map<std::string,JsonParamValSP>::const_iterator it;
	JsonParamValSP jval = std::dynamic_pointer_cast<JsonParamVal>(val);
	m_keys.insert(key);

	if ((it=m_map.find(key)) != m_map.end()) {
		m_map.erase(it);
	}
	m_map.insert({key, jval});
}

nlohmann::json JsonParamValMap::dump() {
	nlohmann::json ret;

	for (std::map<std::string,JsonParamValSP>::const_iterator
			it=m_map.begin();
			it!=m_map.end(); it++) {
		ret[it->first] = it->second->dump();
	}

	return ret;
}

JsonParamValMapSP JsonParamValMap::mk() {
	return JsonParamValMapSP(new JsonParamValMap());
}

JsonParamValMapSP JsonParamValMap::mk(const nlohmann::json &msg) {
	return JsonParamValMapSP(new JsonParamValMap());
}

} /* namespace tblink_rpc_core */
