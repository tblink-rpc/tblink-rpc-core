/*
 * ITransport.h
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */
#pragma once
#include <functional>
#include <stdint.h>

#include "IParamValVec.h"
#include "tblink_rpc/IParamValMap.h"
#include "tblink_rpc/IParamValFactory.h"

namespace tblink_rpc_core {

typedef std::function<int32_t(
		const std::string		&method,
		intptr_t				id,
		IParamValMap			*params)> recv_req_f;

typedef std::function<int32_t(
		intptr_t				id,
		IParamValMap			*result,
		IParamValMap			*error)> recv_rsp_f;

class ITransport : public virtual IParamValFactory {
public:

	virtual ~ITransport() { }

	virtual void init(
			const recv_req_f &req_f,
			const recv_rsp_f &rsp_f) = 0;

	virtual void shutdown() = 0;

	virtual intptr_t send_req(
			const std::string		&method,
			IParamValMap			*params) = 0;

	virtual int32_t send_notify(
			const std::string		&method,
			IParamValMap			*params) = 0;

	virtual int32_t send_rsp(
			intptr_t			id,
			IParamValMap		*result,
			IParamValMap		*error) = 0;

	/**
	 * Polls, waiting for a message. Returns -1 in case of failure.
	 */
	virtual int32_t process_one_message() = 0;

	/**
	 * Waits for at least one message to be received
	 */
	virtual int32_t await_msg() = 0;

	/**
	 * Returns number of mismatched req/rsp pairs
	 */
	virtual int32_t outstanding() = 0;

};

}
