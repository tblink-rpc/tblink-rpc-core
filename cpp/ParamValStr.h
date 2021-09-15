/*
 * ValStr.h
 *
 *  Created on: Sep 30, 2020
 *      Author: ballance
 */

#pragma once
#include <string>

#include "ParamVal.h"
#include "tblink_rpc/IParamValStr.h"

namespace tblink_rpc_core {

class ParamValStr;
typedef std::unique_ptr<ParamValStr> ParamValStrUP;
class ParamValStr : public ParamVal, public virtual IParamValStr {
public:
	ParamValStr(const std::string &v);

	virtual ~ParamValStr();

	virtual std::string val() override { return m_val; }

	const std::string &val() const { return m_val; }

	void val(const std::string &v) { m_val = v; }

	virtual IParamValStr *clone() override;

	static ParamValStrUP mk(const std::string &v);

private:
	std::string				m_val;
};

} /* namespace tblink_rpc_core */

