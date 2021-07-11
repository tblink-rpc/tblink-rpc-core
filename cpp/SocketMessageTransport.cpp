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

#undef EN_DEBUG_SOCKET_MESSAGE_TRANSPORT

#ifdef EN_DEBUG_SOCKET_MESSAGE_TRANSPORT
#define DEBUG_ENTER(fmt, ...) \
	fprintf(stdout, "--> SocketMessageTransport::"); \
	fprintf(stdout, fmt, #__VA_ARGS__); \
	fprintf(stdout, "\n"); \
	fflush(stdout)
#define DEBUG_LEAVE(fmt, ...) \
	fprintf(stdout, "<-- SocketMessageTransport::"); \
	fprintf(stdout, fmt, #__VA_ARGS__); \
	fprintf(stdout, "\n"); \
	fflush(stdout)
#define DEBUG(fmt, ...) \
	fprintf(stdout, "SocketMessageTransport: "); \
	fprintf(stdout, fmt, #__VA_ARGS__); \
	fprintf(stdout, "\n"); \
	fflush(stdout)
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

				fprintf(stdout, "ret=%d\n", ret);

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

		DEBUG("poll -- timeout=%d", timeout_ms);

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

		DEBUG("sz=%d", sz);

		if (sz <= 0) {
			DEBUG("sz=%d errno=%d", sz, errno);

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

		DEBUG("received %d bytes", sz);

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
					DEBUG("State 1: append %c", tmp[i]);
					msgbuf_append(tmp[i]);
					if (isspace(tmp[i])) {
						msgbuf_append(0);
						DEBUG("header=%s", m_msgbuf);
						m_msg_length = strtoul(m_msgbuf, 0, 10);
						DEBUG("len=%d", m_msg_length);
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
						DEBUG("Received message: \"%s\"", m_msgbuf);
						nlohmann::json msg;
						try {
							msg = nlohmann::json::parse(m_msgbuf);

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
								m_req_f(msg["method"], id, params);
								DEBUG_LEAVE("m_req_f");
							} else {
								// TODO: Response
								IParamValMapSP result;
								IParamValMapSP error;
								JsonParamValIntSP id     = JsonParamValInt::mk(msg["id"]);
								DEBUG_ENTER("m_rsp_f");
								m_rsp_f(id->val_s(), result, error);
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

	DEBUG_LEAVE("poll return ret=%d,%d", ret, m_outstanding);
	return (ret || m_outstanding > 0);
}

intptr_t SocketMessageTransport::send_req(
		const std::string		&method,
		IParamValMapSP			params) {
	DEBUG_ENTER("send_req");
	char tmp[64];
	nlohmann::json msg;
	msg["json-rpc"] = "2.0";
	msg["method"] = method;
	msg["id"] = m_id;
	msg["params"] = std::dynamic_pointer_cast<JsonParamValMap>(params)->dump();

	int32_t ret = m_id;
	m_id++;

	std::string body = msg.dump();
	DEBUG("body=\"%s\" len=%d", body.c_str(), body.size());
	sprintf(tmp, "Content-Length: %d\r\n\r\n", body.size());

	m_outstanding += 1;

	::send(m_socket, tmp, strlen(tmp), 0);
	::send(m_socket, body.c_str(), body.size(), 0);

	DEBUG_LEAVE("send_req");
	return ret;
}

int32_t SocketMessageTransport::send_notify(
		const std::string		&method,
		IParamValMapSP			params) {
	DEBUG_ENTER("send_notify\n");
	char tmp[64];
	nlohmann::json msg;
	msg["json-rpc"] = "2.0";
	msg["method"] = method;
	msg["params"] = std::dynamic_pointer_cast<JsonParamValMap>(params)->dump();

	int32_t ret = 0;

	std::string body = msg.dump();
	DEBUG("body=\"%s\" len=%d", body.c_str(), body.size());
	sprintf(tmp, "Content-Length: %d\r\n\r\n", body.size());

	::send(m_socket, tmp, strlen(tmp), 0);
	::send(m_socket, body.c_str(), body.size(), 0);

	DEBUG_LEAVE("send_notify");
	return ret;
}

int32_t SocketMessageTransport::send_rsp(
		intptr_t				id,
		IParamValMapSP			result,
		IParamValMapSP			error) {
	DEBUG_ENTER("send_rsp");
	char tmp[64];
	nlohmann::json msg;
	msg["json-rpc"] = "2.0";
	msg["id"] = id;
	if (result) {
		msg["result"] = std::dynamic_pointer_cast<JsonParamVal>(result)->dump();
	} else {
		msg["error"]  = std::dynamic_pointer_cast<JsonParamVal>(error)->dump();
	}

	std::string body = msg.dump();
	DEBUG("body=\"%s\" len=%d", body.c_str(), body.size());
	sprintf(tmp, "Content-Length: %d\r\n\r\n", body.size());

	int32_t ret = 0;

	ret = ::send(m_socket, tmp, strlen(tmp), 0);
	ret = ::send(m_socket, body.c_str(), body.size(), 0);

	m_outstanding -= 1;

	DEBUG_LEAVE("send_rsp");
	return ret;
}

IParamValBoolSP SocketMessageTransport::mkValBool(bool val) {
	return JsonParamValBool::mk(val);
}

IParamValIntSP SocketMessageTransport::mkValIntU(uint64_t val) {
	return JsonParamValInt::mk(val);
}

IParamValIntSP SocketMessageTransport::mkValIntS(int64_t val) {
	return JsonParamValInt::mk(val);
}

IParamValMapSP SocketMessageTransport::mkValMap() {
	return JsonParamValMap::mk();
}

IParamValStrSP SocketMessageTransport::mkValStr(const std::string &val) {
	return JsonParamValStr::mk(val);
}

IParamValVectorSP SocketMessageTransport::mkVector() {
	return JsonParamValVectorBase::mk();
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

