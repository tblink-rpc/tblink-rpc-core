/*
 * JsonInterfaceType.h
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#pragma once
#include <vector>
#include "tblink_rpc/IInterfaceType.h"
#include "tblink_rpc/IMethodType.h"

namespace tblink_rpc_core {

class JsonInterfaceType;
typedef std::unique_ptr<JsonInterfaceType> JsonInterfaceTypeUP;
class JsonInterfaceType : public IInterfaceType {
public:
	JsonInterfaceType(const std::string &name);

	virtual ~JsonInterfaceType();

	virtual const std::string &name() override {
		return m_name;
	}

	virtual const std::vector<IMethodType *> &methods() {
		return m_methods_p;
	}

	void addMethod(IMethodType *method) {
		m_methods.push_back(IMethodTypeUP(method));
		m_methods_p.push_back(method);
	}

//	virtual IMethodType *getMethodById(int32_t id) = 0;

private:
	std::string						m_name;
	std::vector<IMethodTypeUP>		m_methods;
	std::vector<IMethodType *>		m_methods_p;

};

} /* namespace tblink_rpc_core */

