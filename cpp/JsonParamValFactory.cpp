/*
 * JsonParamValFactory.cpp
 *
 *  Created on: Jul 25, 2021
 *      Author: mballance
 */

#include "JsonParamValFactory.h"

#include "nlohmann/json.hpp"
#include "ParamValBool.h"
#include "ParamValInt.h"
#include "ParamValMap.h"
#include "ParamValStr.h"
#include "ParamValVec.h"

#undef EN_DEBUG_JSON_PARAM_VAL_FACTORY

#ifdef EN_DEBUG_JSON_PARAM_VAL_FACTORY
#define DEBUG_ENTER(fmt, ...) \
	fprintf(stdout, "--> JsonParamValFactory:: "); \
	fprintf(stdout, fmt, ##__VA_ARGS__); \
	fprintf(stdout, "\n")
#define DEBUG(fmt, ...) \
	fprintf(stdout, "JsonParamValFactory: "); \
	fprintf(stdout, fmt, ##__VA_ARGS__); \
	fprintf(stdout, "\n")
#define DEBUG_LEAVE(fmt, ...) \
	fprintf(stdout, "<-- JsonParamValFactory:: "); \
	fprintf(stdout, fmt, ##__VA_ARGS__); \
	fprintf(stdout, "\n")
#else
#define DEBUG(fmt, ...)
#define DEBUG_ENTER(fmt, ...)
#define DEBUG_LEAVE(fmt, ...)
#endif


namespace tblink_rpc_core {

JsonParamValFactory::JsonParamValFactory() {
	// TODO Auto-generated constructor stub

}

JsonParamValFactory::~JsonParamValFactory() {
	// TODO Auto-generated destructor stub
}

IParamVal *JsonParamValFactory::mk(const nlohmann::json &msg) {
	ParamVal *ret = 0;
	DEBUG_ENTER("mk: %s", msg.dump().c_str());

	switch (msg.type()) {
	case nlohmann::json::value_t::number_integer:
		ret = new ParamValInt(msg.get<int64_t>());
		break;
	case nlohmann::json::value_t::number_unsigned:
		ret = new ParamValInt(msg.get<uint64_t>());
		break;
	case nlohmann::json::value_t::object: {
		ParamValMap *mval = new ParamValMap();
		for (auto it=msg.begin(); it!=msg.end(); it++) {
			mval->setVal(it.key(), mk(it.value()));
		}
		ret = mval;
	} break;
	case nlohmann::json::value_t::string:
		ret = new ParamValStr(msg.get<std::string>());
		break;
	case nlohmann::json::value_t::array: {
		ParamValVec *vval = new ParamValVec();
		for (auto it=msg.begin(); it!=msg.end(); it++) {
			vval->push_back(mk(*it));
		}
		ret = vval;
	} break;
	case nlohmann::json::value_t::boolean:
		ret = new ParamValBool(msg.get<bool>());
		break;
	default:
		fprintf(stdout, "Error: unhandled parameter type %s\n", msg.type_name());
	}

	DEBUG_LEAVE("mk: %p %s", ret, msg.dump().c_str());
	return ret;
}

nlohmann::json JsonParamValFactory::mk(IParamVal *val) {
	switch (val->type()) {
	case IParamVal::Bool:
		return nlohmann::json(dynamic_cast<IParamValBool *>(val)->val());
		break;
	case IParamVal::Int:
		return nlohmann::json(dynamic_cast<IParamValInt *>(val)->val_s());
		break;
	case IParamVal::Map: {
		nlohmann::json msg = nlohmann::json::object();
		IParamValMap *mval = dynamic_cast<IParamValMap *>(val);
		for(auto k_it=mval->keys().begin(); k_it!=mval->keys().end(); k_it++) {
			if (!mval->getVal(*k_it)) {
				fprintf(stdout, "map elem %s is null\n", k_it->c_str());
				fflush(stdout);
			}
			msg[*k_it] = mk(mval->getVal(*k_it));
		}
		return msg;
	} break;
	case IParamVal::Str:
		return nlohmann::json(dynamic_cast<IParamValStr *>(val)->val());
		break;
	case IParamVal::Vec: {
		nlohmann::json msg = nlohmann::json::array();
		IParamValVec *vval = dynamic_cast<IParamValVec *>(val);
		for (uint32_t i=0; i<vval->size(); i++) {
			msg.push_back(mk(vval->at(i)));
		}
		return msg;
	} break;
	}

	return nlohmann::json();
}

} /* namespace tblink_rpc_core */
