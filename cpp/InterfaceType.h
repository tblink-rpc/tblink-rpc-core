/*
 * JsonInterfaceType.h
 *
 *  Created on: Jul 12, 2021
 *      Author: mballance
 */

#pragma once
#include <map>
#include <vector>
#include "tblink_rpc/IInterfaceType.h"
#include "tblink_rpc/IMethodType.h"

namespace tblink_rpc_core {

class InterfaceType;
typedef std::unique_ptr<InterfaceType> InterfaceTypeUP;
class InterfaceType : public IInterfaceType {
public:
	InterfaceType(const std::string &name);

	virtual ~InterfaceType();

	virtual const std::string &name() override {
		return m_name;
	}

	virtual const std::vector<IMethodType *> &methods() override {
		return m_methods_p;
	}

	void addMethod(IMethodType *method);

	IMethodType *findMethod(const std::string &name) override;

//	virtual IMethodType *getMethodById(int32_t id) = 0;

private:
	std::string							m_name;
	std::vector<IMethodTypeUP>			m_methods;
	std::vector<IMethodType *>			m_methods_p;
	std::map<std::string,IMethodType*>	m_methods_m;

};

} /* namespace tblink_rpc_core */

