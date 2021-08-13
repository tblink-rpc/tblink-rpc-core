/*
 * SocketMessageTransport.cpp
 *
 *  Created on: Sep 21, 2020
 *      Author: ballance
 */

#include "SocketMessageTransport.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/wait.h>
#endif
#include "nlohmann/json.hpp"

#include "JsonParamValBool.h"
#include "JsonParamValInt.h"
#include "JsonParamValMap.h"
#include "JsonParamValStr.h"
#include "JsonParamValVectorBase.h"
#include "glog/logging.h"

#define EN_DEBUG_SOCKET_MESSAGE_TRANSPORT

#ifdef EN_DEBUG_SOCKET_MESSAGE_TRANSPORT
#define DEBUG_ENTER(fmt, ...) \
	VLOG(1) << "--> SocketMessageTransport::" << fmt
#define DEBUG_LEAVE(fmt, ...) \
	VLOG(1) << "<-- SocketMessageTransport::" << fmt
#define DEBUG(fmt, ...) \
	VLOG(1) << "SocketMessageTransport: " << fmt
#else
#define DEBUG(fmt, ...)
#define DEBUG_ENTER(fmt, ...)
#define DEBUG_LEAVE(fmt, ...)
#endif

namespace tblink_rpc_core {

SocketMessageTransport::SocketMessageTransport(
		pid_t		pid,
		int32_t 	socket) :
	m_msgbuf(0), m_msg_state(0), m_msg_length(0),
	m_pid(pid), m_socket(socket) {
	m_id = 0;

	m_outstanding = 0;

	// TODO Auto-generated constructor stub
	m_msgbuf_idx = 0;
	m_msgbuf_max = 1024;
	m_msgbuf = new char[m_msgbuf_max];
}

SocketMessageTransport::~SocketMessageTransport() {
	if (m_msgbuf) {
		delete [] m_msgbuf;
	}
}

void SocketMessageTransport::init(
		const recv_req_f			&req_f,
		const recv_rsp_f			&rsp_f) {
	m_req_f = req_f;
	m_rsp_f = rsp_f;
}

void SocketMessageTransport::shutdown() {
	DEBUG_ENTER("shutdown");
	if (m_pid > 0) {
		int status = -1;
		for (uint32_t i=0; i<5; i++) {
			int ret = ::waitpid(m_pid, &status, WNOHANG);

			if (ret == 0) {
				DEBUG("Sleeping...");
				::sleep(1);
			} else if (ret == m_pid) {
				DEBUG("ended");
				m_pid = 0;
				break;
			}
		}

		if (m_pid > 0) {
			fprintf(stdout, "TODO: terminate process\n");

			for (uint32_t i=0; i<5; i++) {
				int ret = ::kill(m_pid, (i<3)?SIGTERM:SIGKILL);

				ret = ::waitpid(m_pid, &status, WNOHANG);

				DEBUG("waitpid ret=") << ret;

				if (ret == m_pid) {
					break;
				} else {
					DEBUG("Sleeping...");
					::sleep(1);
				}
			}
		}
	}
}

int32_t SocketMessageTransport::poll(int timeout_ms) {
	char tmp[1024];
	int32_t sz;
	int32_t ret = 0;

	// Poll for data

	do {

		DEBUG("poll -- timeout=") << timeout_ms;

		if (timeout_ms > 0) {
			// Select to wait on data
			fd_set rfds;
			struct timeval tv;
			int retval;

			FD_ZERO(&rfds);
			FD_SET(m_socket, &rfds);

			/* Wait up to five seconds. */

			tv.tv_sec = timeout_ms/1000;
			tv.tv_usec = (timeout_ms%1000)*1000;

			retval = select(m_socket+1, &rfds, 0, 0, &tv);

			if (retval == -1) {
				fprintf(stdout, "Error: select %d\n", errno);
				ret = -1;
				break;
			} else if (retval == 0) {
				// No data
				DEBUG("Note: no data");
				timeout_ms = 0;
				break;
			} else {
				timeout_ms = 0;
			}
		}

		DEBUG_ENTER("poll recv");
		sz = ::recv(m_socket, tmp, 1024, 0);
		DEBUG_LEAVE("poll recv");

		DEBUG("sz=") << sz;

		if (sz <= 0) {
			DEBUG("sz=") << sz << " errno=" << errno;

			if (errno == EAGAIN) {
				// Just nothing to see here
				ret = 0;
			} else {
				ret = -1;
			}
			break;
		} else {
			ret = 1;
		}

		DEBUG("received ") << sz << " bytes";

		// Process data
		for (uint32_t i=0; i<sz; i++) {
//			fprintf(stdout, "[%d] Process char %c\n", m_msg_state, tmp[i]);
			switch (m_msg_state) {
			case 0: { // Waiting for a header
				if (tmp[i] == HEADER_PREFIX.at(m_msgbuf_idx)) {
					m_msgbuf_idx++;
				} else {
					m_msgbuf_idx = 0;
				}
				if (m_msgbuf_idx == HEADER_PREFIX.size()) {
					m_msgbuf_idx = 0;
					m_msg_state = 1;
				}
			} break;

			case 1: { // Collecting length up to first '\n'
				if (m_msgbuf_idx == 0 && isspace(tmp[i])) {
					// Skip leading whitespace
				} else {
					DEBUG("State 1: append ") << tmp[i];
					msgbuf_append(tmp[i]);
					if (isspace(tmp[i])) {
						msgbuf_append(0);
						DEBUG("header=") << m_msgbuf;
						m_msg_length = strtoul(m_msgbuf, 0, 10);
						DEBUG("len=") << m_msg_length;
						// Reset the buffer to collect the payload
						m_msgbuf_idx = 0;
						m_msg_state = 2;
					}
				}

			} break;

			case 2: { // Collecting body data
				if (m_msgbuf_idx == 0 && isspace(tmp[i])) {
					// Skip leading whitespace
				} else {
					msgbuf_append(tmp[i]);
					if (m_msgbuf_idx >= m_msg_length) {
						msgbuf_append(0);
						DEBUG("Received message: \"") << m_msgbuf << "\"";
						nlohmann::json msg;
						try {
							msg = nlohmann::json::parse(m_msgbuf);

							// Is this a request or a response?
							if (msg.contains("method")) {
								// TODO: Request or notify
								JsonParamValMapUP params = JsonParamValMap::mk(msg["params"]);

								intptr_t id = -1;

								if (msg.contains("id")) {
									id = msg["id"];
								}

								DEBUG_ENTER("m_req_f");
								m_outstanding += 1;
								m_req_f(msg["method"], id, params.get());
								DEBUG_LEAVE("m_req_f");
							} else {
								// TODO: Response
								IParamValMapUP result;
								IParamValMapUP error;
								JsonParamValIntUP id = JsonParamValInt::mk(msg["id"]);
								DEBUG_ENTER("m_rsp_f");
								if (msg.contains("result")) {
									result = JsonParamValMap::mk(msg["result"]);
								} else if (msg.contains("error")) {
									error = JsonParamValMap::mk(msg["error"]);
								}
								m_rsp_f(id->val_s(), result.get(), error.get());
								m_outstanding -= 1;
								DEBUG_LEAVE("m_rsp_f");
							}
						} catch (const std::exception &e) {
							fprintf(stdout, "Failed to parse msg \"%s\" %s\n",
									m_msgbuf, e.what());
						}
						m_msg_state = 0;
						m_msgbuf_idx = 0;
					}
				}
			} break;

			default: {
				m_msgbuf_idx = 0;
				m_msg_state = 0;
			}
			}
		}
	} while (ret == 0 || timeout_ms > 0);

	DEBUG_LEAVE("poll return ret=") << ret << "," << m_outstanding;
	if (ret == -1) {
		return -1;
	} else {
		return (ret || m_outstanding > 0);
	}
}

int32_t SocketMessageTransport::await_msg() {
	int32_t ret = 0;
	char tmp[1024];

	int32_t have_msg = 0;
	do {
		int32_t timeout_ms = 1000;

		// Select to wait on data
		int retval;
		do {
			fd_set rfds;
			struct timeval tv;

			FD_ZERO(&rfds);
			FD_SET(m_socket, &rfds);

			tv.tv_sec = timeout_ms/1000;
			tv.tv_usec = (timeout_ms%1000)*1000;

			retval = select(m_socket+1, &rfds, 0, 0, &tv);

		} while (retval == 0 || (retval == -1 && errno == EAGAIN));

		if (retval == -1) {
			ret = -1;
			break;
		}

		DEBUG_ENTER("poll recv");
		int32_t sz = ::recv(m_socket, tmp, sizeof(tmp), 0);
		DEBUG_LEAVE("poll recv");

		if (sz == -1 && errno != EAGAIN) {
			ret = -1;
		} else if (sz > 0) {
			ret = process_data(tmp, sz);
		}
	} while (ret == 0);

	return ret;
}

intptr_t SocketMessageTransport::send_req(
		const std::string		&method,
		IParamValMap			*params) {
	DEBUG_ENTER("send_req");
	char tmp[64];
	nlohmann::json msg;
	msg["json-rpc"] = "2.0";
	msg["method"] = method;
	msg["id"] = m_id;
	msg["params"] = dynamic_cast<JsonParamValMap *>(params)->dump();

	int32_t ret = m_id;
	m_id++;

	std::string body = msg.dump();
	DEBUG("body=\"") << body << " len=" << body.size();
	sprintf(tmp, "Content-Length: %d\r\n\r\n", body.size());

	m_outstanding += 1;

	::send(m_socket, tmp, strlen(tmp), 0);
	::send(m_socket, body.c_str(), body.size(), 0);

	// Ownership transfers back to us
	delete params;

	DEBUG_LEAVE("send_req");
	return ret;
}

int32_t SocketMessageTransport::send_notify(
		const std::string		&method,
		IParamValMap			*params) {
	DEBUG_ENTER("send_notify\n");
	char tmp[64];
	nlohmann::json msg;
	msg["json-rpc"] = "2.0";
	msg["method"] = method;
	msg["params"] = dynamic_cast<JsonParamValMap *>(params)->dump();

	int32_t ret = 0;

	std::string body = msg.dump();
	DEBUG("body=\"") << body << "\" len=" << body.size();
	sprintf(tmp, "Content-Length: %d\r\n\r\n", body.size());

	::send(m_socket, tmp, strlen(tmp), 0);
	::send(m_socket, body.c_str(), body.size(), 0);

	delete params;

	DEBUG_LEAVE("send_notify");
	return ret;
}

int32_t SocketMessageTransport::send_rsp(
		intptr_t				id,
		IParamValMap			*result,
		IParamValMap			*error) {
	DEBUG_ENTER("send_rsp");
	char tmp[64];
	nlohmann::json msg;
	msg["json-rpc"] = "2.0";
	msg["id"] = id;
	if (result) {
		msg["result"] = dynamic_cast<JsonParamVal *>(result)->dump();
		delete result;
	} else {
		msg["error"]  = dynamic_cast<JsonParamVal *>(error)->dump();
		delete error;
	}

	std::string body = msg.dump();
	DEBUG("body=\"") << body << "\" len=" << body.size();
	sprintf(tmp, "Content-Length: %d\r\n\r\n", body.size());

	int32_t ret = 0;

	ret = ::send(m_socket, tmp, strlen(tmp), 0);
	ret = ::send(m_socket, body.c_str(), body.size(), 0);

	m_outstanding -= 1;

	DEBUG_LEAVE("send_rsp");
	return ret;
}

IParamValBool *SocketMessageTransport::mkValBool(bool val) {
	return JsonParamValBool::mk(val).release();
}

IParamValInt *SocketMessageTransport::mkValIntU(uint64_t val) {
	return JsonParamValInt::mk(val).release();
}

IParamValInt *SocketMessageTransport::mkValIntS(int64_t val) {
	return JsonParamValInt::mk(val).release();
}

IParamValMap *SocketMessageTransport::mkValMap() {
	return JsonParamValMap::mk().release();
}

IParamValStr *SocketMessageTransport::mkValStr(const std::string &val) {
	return JsonParamValStr::mk(val).release();
}

IParamValVector *SocketMessageTransport::mkVector() {
	return JsonParamValVectorBase::mk().release();
}

int32_t SocketMessageTransport::process_data(char *data, uint32_t sz) {
	int32_t ret = 0;

	// Process data
	for (uint32_t i=0; i<sz; i++) {
		switch (m_msg_state) {
		case 0: { // Waiting for a header
			if (data[i] == HEADER_PREFIX.at(m_msgbuf_idx)) {
				m_msgbuf_idx++;
			} else {
				m_msgbuf_idx = 0;
			}
			if (m_msgbuf_idx == HEADER_PREFIX.size()) {
				m_msgbuf_idx = 0;
				m_msg_state = 1;
			}
		} break;

		case 1: { // Collecting length up to first '\n'
			if (m_msgbuf_idx == 0 && isspace(data[i])) {
				// Skip leading whitespace
			} else {
				DEBUG("State 1: append ") << data[i];
				msgbuf_append(data[i]);
				if (isspace(data[i])) {
					msgbuf_append(0);
					DEBUG("header=") << m_msgbuf;
					m_msg_length = strtoul(m_msgbuf, 0, 10);
					DEBUG("len=") << m_msg_length;
					// Reset the buffer to collect the payload
					m_msgbuf_idx = 0;
					m_msg_state = 2;
				}
			}

		} break;

		case 2: { // Collecting body data
			if (m_msgbuf_idx == 0 && isspace(data[i])) {
				// Skip leading whitespace
			} else {
				msgbuf_append(data[i]);
				if (m_msgbuf_idx >= m_msg_length) {
					msgbuf_append(0);
					DEBUG("Received message: \"") << m_msgbuf << "\"";
					nlohmann::json msg;
					try {
						msg = nlohmann::json::parse(m_msgbuf);

						// We received a message
						ret = 1;

						// Is this a request or a response?
						if (msg.contains("method")) {
							// TODO: Request or notify
							JsonParamValMapSP params = JsonParamValMap::mk(msg["params"]);

							intptr_t id = -1;

							if (msg.contains("id")) {
								id = msg["id"];
							}

							DEBUG_ENTER("m_req_f");
							m_outstanding += 1;
							m_req_f(msg["method"], id, params.get());
							DEBUG_LEAVE("m_req_f");
						} else {
							// TODO: Response
							IParamValMapUP result;
							IParamValMapUP error;
							JsonParamValIntUP id = JsonParamValInt::mk(msg["id"]);
							DEBUG_ENTER("m_rsp_f");
							if (msg.contains("result")) {
								result = JsonParamValMap::mk(msg["result"]);
							} else if (msg.contains("error")) {
								error = JsonParamValMap::mk(msg["error"]);
							}
							// Parameters pass to the callee
							m_rsp_f(id->val_s(), result.release(), error.release());
							m_outstanding -= 1;
							DEBUG_LEAVE("m_rsp_f");
						}
					} catch (const std::exception &e) {
						fprintf(stdout, "Failed to parse msg \"%s\" %s\n",
								m_msgbuf, e.what());
					}
					m_msg_state = 0;
					m_msgbuf_idx = 0;
				}
			}
		} break;

		default: {
			m_msgbuf_idx = 0;
			m_msg_state = 0;
		}
		}
	}

	return ret;
}

void SocketMessageTransport::msgbuf_resize_append(char c) {
	// Confirm that we need to resize
	if (m_msgbuf_idx+1 >= m_msgbuf_max) {
		// yup, must resize
		char *tmp = m_msgbuf;
		m_msgbuf = new char[m_msgbuf_max+1024];

		memcpy(m_msgbuf, tmp, m_msgbuf_idx);
		m_msgbuf_max += 1024;

		delete [] tmp;
	}

	m_msgbuf[m_msgbuf_idx++] = c;
}

const std::string	SocketMessageTransport::HEADER_PREFIX = "Content-Length: ";

} /* namespace tblink_rpc_core */

