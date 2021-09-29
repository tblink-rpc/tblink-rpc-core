/*
 * JsonInterfaceType.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#include "InterfaceType.h"

namespace tblink_rpc_core {

InterfaceType::InterfaceType(const std::string &name) : m_name(name) {
	// TODO Auto-generated constructor stub

}

InterfaceType::~InterfaceType() {
	// TODO Auto-generated destructor stub
}

void InterfaceType::addMethod(IMethodType *method) {
	fprintf(stdout, "addMethod: %p\n", method);
	fflush(stdout);
	m_methods.push_back(IMethodTypeUP(method));
	m_methods_p.push_back(method);
	m_methods_m.insert({method->name(), method});
}

IMethodType *InterfaceType::findMethod(const std::string &name) {
	std::map<std::string,IMethodType*>::const_iterator it;

	if ((it=m_methods_m.find(name)) != m_methods_m.end()) {
		return it->second;
	} else {
		return 0;
	}
}

} /* namespace tblink_rpc_core */
