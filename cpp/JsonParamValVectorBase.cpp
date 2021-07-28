/*
 * ValVector.cpp
 *
 *  Created on: Oct 1, 2020
 *      Author: ballance
 */

#include "nlohmann/json.hpp"
#include "JsonParamValFactory.h"
#include "JsonParamValVectorBase.h"

namespace tblink_rpc_core {

JsonParamValVectorBase::JsonParamValVectorBase() {

}

JsonParamValVectorBase::JsonParamValVectorBase(
		std::function<JsonParamValSP(const nlohmann::json &)> 	ctor,
		const nlohmann::json							&msg) {
	/*
	for (uint32_t i=0; ; ) {
		ValSP v = ctor(msg);
	}
	 */

}

JsonParamValVectorBase::JsonParamValVectorBase(const nlohmann::json &msg) {
	fprintf(stdout, "JsonParamValVectorBase: sz=%d %d %s\n",
			msg.size(),
			(msg.begin() == msg.end()),
			msg.dump().c_str());
	for (nlohmann::json::const_iterator
			it=msg.begin();
			it!=msg.end(); it++) {
		m_children.push_back(JsonParamValFactory::mk(*it));
	}
}

JsonParamValVectorBase::~JsonParamValVectorBase() {
	// TODO Auto-generated destructor stub
}

void JsonParamValVectorBase::push_back(IParamValSP v) {
	m_children.push_back(std::dynamic_pointer_cast<JsonParamVal>(v));
}

nlohmann::json JsonParamValVectorBase::dump() {
	nlohmann::json msg;

	fprintf(stdout, "Dump: %d children\n", m_children.size());

	if (m_children.size() == 0) {
		msg = nlohmann::json::array();
	} else {
		for (std::vector<JsonParamValSP>::const_iterator
			it=m_children.begin();
			it!=m_children.end(); it++) {
			msg.push_back((*it)->dump());
		}
	}

	return msg;
}

JsonParamValVectorBaseSP JsonParamValVectorBase::mk(
			std::function<JsonParamValSP(const nlohmann::json &)> 	ctor,
			const nlohmann::json 							&msg) {
	return JsonParamValVectorBaseSP(new JsonParamValVectorBase(ctor, msg));
}

JsonParamValVectorBaseSP JsonParamValVectorBase::mk() {
	return JsonParamValVectorBaseSP(new JsonParamValVectorBase());
}

} /* namespace tblink_rpc_core */
