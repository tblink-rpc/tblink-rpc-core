/*
 * FifoMessageTransportEndpointEP.h
 *
 *  Created on: Aug 13, 2021
 *      Author: mballance
 */

#pragma once
#include <memory>
#include <vector>
#include "tblink_rpc/ITransport.h"

namespace tblink_rpc_core {

class TransportMessageFifo;

class TransportMessageFifoUP;
typedef std::unique_ptr<TransportMessageFifoUP> FifoMessageTransportEPUP;
class TransportMessageFifoUP : public virtual ITransport {
	friend class TransportMessageFifo;
public:
	TransportMessageFifoUP(
			TransportMessageFifo		*parent,
			uint32_t					id);

	virtual ~TransportMessageFifoUP();

	virtual void init(
			const recv_req_f &req_f,
			const recv_rsp_f &rsp_f) override;

	virtual void shutdown() override;

	virtual intptr_t send_req(
			const std::string		&method,
			IParamValMap			*params) override;

	virtual int32_t send_notify(
			const std::string		&method,
			IParamValMap			*params) override;

	virtual int32_t send_rsp(
			intptr_t			id,
			IParamValMap		*result,
			IParamValMap		*error) override;

	virtual int32_t poll(int32_t timeout_ms=-1) override;

	/**
	 * Waits for at least one message to be received
	 */
	virtual int32_t await_msg() override;

	/**
	 * Returns number of mismatched req/rsp pairs
	 */
	virtual int32_t outstanding() override;

	virtual IParamValBool *mkValBool(bool val) override;

	virtual IParamValInt *mkValIntU(uint64_t val) override;

	virtual IParamValInt *mkValIntS(int64_t val) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVec *mkValVec() override;

private:

	typedef std::tuple<std::string,intptr_t,IParamValMap*>   req_elem_t;
	typedef std::tuple<intptr_t,IParamValMap*,IParamValMap*> rsp_elem_t;

	int32_t recv_req(
			const std::string		&method,
			intptr_t				req_id,
			IParamValMap			*params);

	int32_t recv_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error);



private:
	TransportMessageFifo			*m_parent;
	uint32_t						m_id;
	recv_req_f						m_req_f;
	recv_rsp_f						m_rsp_f;
	intptr_t						m_req_id;
	std::vector<req_elem_t>			m_req_q;
	std::vector<rsp_elem_t>			m_rsp_q;
};

} /* namespace tblink_rpc_core */

