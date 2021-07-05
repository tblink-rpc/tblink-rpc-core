/*
 * JsonParamValMap.cpp
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */

#include "JsonParamValMap.h"

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
	std::map<std::string,IParamValSP>::const_iterator it;

	if ((it=m_map.find(key)) != m_map.end()) {
		return it->second;
	} else {
		return 0;
	}
}

void JsonParamValMap::setVal(
		const std::string		&key,
		IParamValSP				val) {
	std::map<std::string,IParamValSP>::const_iterator it;
	m_keys.insert(key);

	if ((it=m_map.find(key)) != m_map.end()) {
		m_map.erase(it);
	}
	m_map.insert({key, val});
}

JsonParamValMapSP JsonParamValMap::mk() {
	return JsonParamValMapSP(new JsonParamValMap());
}

} /* namespace tblink_rpc_core */
