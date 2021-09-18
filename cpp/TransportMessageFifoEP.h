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

class TransportMessageFifoEP;
typedef std::unique_ptr<TransportMessageFifoEP> TransportMessageFifoEPUP;
class TransportMessageFifoEP : public virtual ITransport {
	friend class TransportMessageFifo;
public:
	TransportMessageFifoEP(
			TransportMessageFifo		*parent,
			uint32_t					id);

	virtual ~TransportMessageFifoEP();

	virtual TransportMessageFifo *parent() const { return m_parent; }

	virtual void parent(TransportMessageFifo *p) { m_parent = p; }

	virtual uint32_t id() const { return m_id; }

	virtual void id(uint32_t i) { m_id = i; }

	virtual void init(
			const recv_req_f &req_f,
			const recv_rsp_f &rsp_f) override;

	virtual void shutdown() override;

	virtual intptr_t send_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMap			*params) override;

	virtual int32_t send_notify(
			const std::string		&method,
			IParamValMap			*params) override;

	virtual int32_t send_rsp(
			intptr_t			id,
			IParamValMap		*result,
			IParamValMap		*error) override;

	/**
	 * Polls, waiting for a message. Returns -1 in case of failure.
	 */
	virtual int32_t process_one_message() override;

	/**
	 * Waits for at least one message to be received
	 */
	virtual int32_t await_msg() override;

	/**
	 * Returns number of mismatched req/rsp pairs
	 */
	virtual int32_t outstanding() override;

	virtual IParamValBool *mkValBool(bool val) override;

	virtual IParamValInt *mkValIntU(uint64_t val, int32_t width) override;

	virtual IParamValInt *mkValIntS(int64_t val, int32_t width) override;

	virtual IParamValMap *mkValMap() override;

	virtual IParamValStr *mkValStr(const std::string &val) override;

	virtual IParamValVec *mkValVec() override;

protected:

	typedef std::tuple<std::string,intptr_t,IParamValMap*>   req_elem_t;
	typedef std::tuple<intptr_t,IParamValMap*,IParamValMap*> rsp_elem_t;

	virtual int32_t recv_req(
			const std::string		&method,
			intptr_t				req_id,
			IParamValMap			*params);

	virtual int32_t recv_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error);



protected:
	TransportMessageFifo			*m_parent;
	uint32_t						m_id;
	recv_req_f						m_req_f;
	recv_rsp_f						m_rsp_f;
	intptr_t						m_req_id;
	std::vector<req_elem_t>			m_req_q;
	std::vector<rsp_elem_t>			m_rsp_q;
};

} /* namespace tblink_rpc_core */

