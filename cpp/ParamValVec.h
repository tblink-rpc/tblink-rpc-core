/*
 * ValVector.h
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */

#pragma once
#include <functional>
#include <vector>

#include "include/tblink_rpc/IParamValVec.h"
#include "ParamVal.h"

namespace tblink_rpc_core {

class ParamValVec;
typedef std::unique_ptr<ParamValVec> ParamValVecUP;
class ParamValVec : public ParamVal, public virtual IParamValVec {
public:
	ParamValVec();

	virtual ~ParamValVec();

	virtual uint32_t size() override { return m_children.size(); }

	virtual IParamVal *at(uint32_t idx) override { return m_children.at(idx).get(); }

	virtual void push_back(IParamVal *v) override;

	virtual IParamValVec *clone() override;

	const std::vector<ParamValUP> &children() const {
		return m_children;
	}

	ParamVal *children(uint32_t idx) const {
		return m_children.at(idx).get();
	}

	static ParamValVecUP mk();

private:
	std::vector<ParamValUP>				m_children;

};


} /* namespace tblink_rpc_core */

