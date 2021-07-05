/*
 * ITransport.h
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */
#pragma once
#include <functional>
#include <stdint.h>
#include "tblink_rpc/IParamValMap.h"
#include "tblink_rpc/IParamValVector.h"
#include "tblink_rpc/IParamValFactory.h"

namespace tblink_rpc_core {

typedef std::function<int32_t(
		const std::string		&method,
		intptr_t				id,
		IParamValMapSP			params)> recv_req_f;

typedef std::function<int32_t(
		intptr_t				id,
		IParamValSP				result,
		IParamValSP				error)> recv_rsp_f;

class ITransport : public virtual IParamValFactory {
public:

	virtual ~ITransport() { }

	virtual void init(
			const recv_req_f &req_f,
			const recv_rsp_f &rsp_f) = 0;

	virtual intptr_t send_req(
			const std::string		&method,
			IParamValMapSP			params) = 0;

	virtual int32_t send_notify(
			const std::string		&method,
			IParamValMapSP			params) = 0;

	virtual int32_t send_rsp(
			intptr_t			id,
			IParamValSP			result,
			IParamValSP			error) = 0;

	virtual int32_t poll(int32_t timeout_ms=-1) = 0;

};

}
