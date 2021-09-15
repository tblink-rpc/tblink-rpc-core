/*
 * JsonParamValMap.cpp
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */

#include "ParamValInt.h"
#include "ParamValMap.h"

#include "JsonParamValFactory.h"
#include "ParamValStr.h"

namespace tblink_rpc_core {

ParamValMap::ParamValMap() :
	ParamVal(IParamVal::Map) {
	// TODO Auto-generated constructor stub

}

ParamValMap::~ParamValMap() {
	// TODO Auto-generated destructor stub
}

const std::set<std::string> &ParamValMap::keys() {
	return m_keys;
}

bool ParamValMap::hasKey(const std::string &key) {
	return (m_map.find(key) != m_map.end());
}

IParamVal *ParamValMap::getVal(
		const std::string		&key) {
	std::unordered_map<std::string,ParamValUP>::const_iterator it;

	if ((it=m_map.find(key)) != m_map.end()) {
		return it->second.get();
	} else {
		return 0;
	}
}

void ParamValMap::setVal(
		const std::string		&key,
		IParamVal				*val) {
	std::unordered_map<std::string,ParamValUP>::const_iterator it;
	ParamVal *jval = dynamic_cast<ParamVal *>(val);
	m_keys.insert(key);

	if ((it=m_map.find(key)) != m_map.end()) {
		m_map.erase(it);
	}
	m_map.insert({key, ParamValUP(jval)});
}

IParamValMap *ParamValMap::clone() {
	ParamValMap *ret = new ParamValMap();

	for (std::unordered_map<std::string,ParamValUP>::const_iterator
			it=m_map.begin();
			it!=m_map.end(); it++) {
		ret->setVal(it->first, it->second->clone());
	}

	return ret;
}

ParamValMapUP ParamValMap::mk() {
	return ParamValMapUP(new ParamValMap());
}

} /* namespace tblink_rpc_core */
