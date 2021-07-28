/*
 * SocketMessageTransport.h
 *
 *  Created on: Sep 21, 2020
 *      Author: ballance
 */

#pragma once
#include <string>
#include "tblink_rpc/ITransport.h"
#include "nlohmann/json_fwd.hpp"

namespace tblink_rpc_core {

class SocketMessageTransport : public virtual ITransport {
public:
	SocketMessageTransport(
			pid_t			pid,
			int32_t			socket);

	virtual ~SocketMessageTransport();

	virtual void init(
			const recv_req_f		&req_f,
			const recv_rsp_f		&rsp_f) override;

	virtual void shutdown() override;

	virtual intptr_t send_req(
			const std::string	&method,
			IParamValMapSP		params) override;

	virtual int32_t send_notify(
			const std::string	&method,
			IParamValMapSP		params) override;

	virtual int32_t send_rsp(
			intptr_t			id,
			IParamValMapSP		result,
			IParamValMapSP		error) override;

	virtual int32_t poll(int32_t timeout_ms=-1) override;

	virtual int32_t await_msg() override;

	virtual int32_t outstanding() override { return m_outstanding; }

	virtual IParamValBoolSP mkValBool(bool val) override;

	virtual IParamValIntSP mkValIntU(uint64_t val) override;

	virtual IParamValIntSP mkValIntS(int64_t val) override;

	virtual IParamValMapSP mkValMap() override;

	virtual IParamValStrSP mkValStr(const std::string &val) override;

	virtual IParamValVectorSP mkVector() override;

private:
	int32_t process_data(char *data, uint32_t sz);

private:
	void msgbuf_append(char c) {
		if (m_msgbuf_idx+1 < m_msgbuf_max) {
			m_msgbuf[m_msgbuf_idx++] = c;
		} else {
			msgbuf_resize_append(c);
		}
	}

	void msgbuf_resize_append(char c);

private:
	char						*m_msgbuf;
	intptr_t					m_id;
	uint32_t					m_msgbuf_idx;
	uint32_t					m_msgbuf_max;
	uint32_t					m_msg_state;
	uint32_t					m_msg_length;
	pid_t						m_pid;
	int32_t						m_socket;
	int32_t						m_outstanding;

	recv_req_f					m_req_f;
	recv_rsp_f					m_rsp_f;



	static const std::string	HEADER_PREFIX;

};

} /* namespace tblink_rpc_core */

