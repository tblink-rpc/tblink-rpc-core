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
typedef std::unique_ptr<IParamValMap> IParamValMapUP;
class IParamValMap : public virtual IParamVal {
public:

	virtual ~IParamValMap() { }

	virtual const std::set<std::string> &keys() = 0;

	virtual bool hasKey(const std::string &key) = 0;

	virtual IParamVal *getVal(
			const std::string		&key) = 0;

	template <class T> T *getValT(
			const std::string		&key) {
		return dynamic_cast<T *>(getVal(key));
	}

	virtual void setVal(
			const std::string		&key,
			IParamVal				*val) = 0;

	virtual IParamValMap *clone() = 0;


};

}
