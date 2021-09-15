/*
 * ValInt.h
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */

#pragma once
#include "ParamVal.h"
#include "tblink_rpc/IParamValInt.h"

namespace tblink_rpc_core {

class ParamValInt;
typedef std::unique_ptr<ParamValInt> ParamValIntUP;
class ParamValInt : public ParamVal, public virtual IParamValInt {
public:
	ParamValInt(int64_t v);

	virtual ~ParamValInt();

	int64_t val() const { return m_val; }

	void val(int64_t v) { m_val = v; }

	virtual uint64_t val_u() override { return m_val; }

	virtual int64_t val_s() override { return m_val; }

	virtual IParamValInt *clone() override;

	static ParamValIntUP mk(int32_t v);

private:
	int64_t					m_val;

};

} /* namespace tblink_rpc_core */

