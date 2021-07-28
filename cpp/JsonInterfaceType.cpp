/*
 * JsonInterfaceType.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#include "JsonInterfaceType.h"

namespace tblink_rpc_core {

JsonInterfaceType::JsonInterfaceType(const std::string &name) : m_name(name) {
	// TODO Auto-generated constructor stub

}

JsonInterfaceType::~JsonInterfaceType() {
	// TODO Auto-generated destructor stub
}

void JsonInterfaceType::addMethod(IMethodType *method) {
	m_methods.push_back(IMethodTypeUP(method));
	m_methods_p.push_back(method);
	m_methods_m.insert({method->name(), method});
}

IMethodType *JsonInterfaceType::findMethod(const std::string &name) {
	std::map<std::string,IMethodType*>::const_iterator it;

	if ((it=m_methods_m.find(name)) != m_methods_m.end()) {
		return it->second;
	} else {
		return 0;
	}
}

} /* namespace tblink_rpc_core */
