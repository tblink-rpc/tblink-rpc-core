/*
 * IParamRecordVal.h
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */

#pragma once
#include <set>
#include "tblink_rpc/IParamVal.h"

namespace tblink_rpc_core {

class IParamValMap;
typedef std::shared_ptr<IParamValMap> IParamValMapSP;
class IParamValMap : public IParamVal {
public:

	virtual ~IParamValMap() { }

	virtual const std::set<std::string> &keys() = 0;

	virtual bool hasKey(const std::string &key) = 0;

	virtual IParamValSP getVal(
			const std::string		&key) = 0;

	template <class T> std::shared_ptr<T> getValT(
			const std::string		&key) {
		return std::dynamic_pointer_cast<T>(getVal(key));
	}

	virtual void setVal(
			const std::string		&key,
			IParamValSP				val) = 0;


};

}
