/*
 * EndpointBase.cpp
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

#include "EndpointBase.h"

#include "InterfaceInstBase.h"
#include "InterfaceType.h"
#include "InterfaceTypeBuilder.h"
#include "ParamValBool.h"
#include "ParamValInt.h"
#include "ParamValMap.h"
#include "ParamValStr.h"
#include "ParamValVec.h"

namespace tblink_rpc_core {

EndpointBase::EndpointBase() : m_services(0), m_listener(0) {
	// TODO Auto-generated constructor stub

}

EndpointBase::~EndpointBase() {
	// TODO Auto-generated destructor stub
}

int32_t EndpointBase::init(
			IEndpointServices		*ep_services,
			IEndpointListener		*ep_listener) {
	m_services = ep_services;
	m_listener = ep_listener;

	return 0;
}

const std::vector<std::string> &EndpointBase::args() {
	return m_services->args();
}

uint64_t EndpointBase::time() {
	if (m_services) {
		return m_services->time();
	} else {
		return 0;
	}
}

int32_t EndpointBase::time_precision() {
	return m_services->time_precision();
}

IInterfaceType *EndpointBase::findInterfaceType(
		const std::string		&name) {
	std::unordered_map<std::string,IInterfaceTypeUP>::const_iterator it;

	if ((it=m_iftype_m.find(name)) != m_iftype_m.end()) {
		return it->second.get();
	} else {
		return 0;
	}
}

IInterfaceTypeBuilder *EndpointBase::newInterfaceTypeBuilder(
		const std::string		&name) {
	return new InterfaceTypeBuilder(name);
}

IInterfaceType *EndpointBase::defineInterfaceType(
		IInterfaceTypeBuilder	*type) {
	InterfaceTypeBuilder *builder =
			static_cast<InterfaceTypeBuilder *>(type);
	InterfaceType *iftype = builder->type();
	m_iftype_m.insert({iftype->name(), IInterfaceTypeUP(iftype)});
	m_iftypes.push_back(iftype);

	return iftype;
}

IParamValBool *EndpointBase::mkValBool(bool val) {
	return new ParamValBool();
}

IParamValInt *EndpointBase::mkValIntU(uint64_t val, int32_t width) {
	return new ParamValInt(val);
}

IParamValInt *EndpointBase::mkValIntS(int64_t val, int32_t width) {
	return new ParamValInt(val);
}

IParamValMap *EndpointBase::mkValMap() {
	return new ParamValMap();
}

IParamValStr *EndpointBase::mkValStr(const std::string &val) {
	return new ParamValStr(val);
}

IParamValVec *EndpointBase::mkValVec() {
	return new ParamValVec();
}

} /* namespace tblink_rpc_core */
