/*
 * tblink_rpc_transport_json_socket.c
 *
 *  Created on: Sep 22, 2021
 *      Author: mballance
 */
#include "tblink_rpc_transport_json_socket.h"

typedef struct tblink_rpc_TransportJsonSocket_s {
	tblink_rpc_ITransport_t			api;
	int32_t							pid;
	int32_t							sock;
	tblink_rpc_recv_req_f			*req_f;
	tblink_rpc_recv_req_f			*rsp_f;
	void							*user_data;
} tblink_rpc_TransportJsonSocket_t;

static void tblink_rpc_TransportJsonSocket_dispose(
		struct tblink_rpc_ITransport_s		*transport) {
	// TODO: close socket (?)
	free(transport);
}

static void tblink_rpc_TransportJsonSocket_init(
		struct tblink_rpc_ITransport_s		*transport,
		tblink_rpc_recv_req_f				*req_f,
		tblink_rpc_recv_rsp_f				*rsp_f,
		void								*user_data) {
	tblink_rpc_TransportJsonSocket_t *jt = (tblink_rpc_TransportJsonSocket_t *)transport;
	jt->req_f = req_f;
	jt->rsp_f = rsp_f;
	jt->user_data = user_data;
}

static int32_t tblink_rpc_TransportJsonSocket_send_req(
		struct tblink_rpc_ITransport_s		*transport,
		const char							*method,
		intptr_t							id,
		tblink_rpc_IParamValMap_t			*params
		) {
	int32_t ret = 0;
	tblink_rpc_TransportJsonSocket_t *jt = (tblink_rpc_TransportJsonSocket_t *)transport;

	return ret;
}

static int32_t tblink_rpc_TransportJsonSocket_send_rsp(
		struct tblink_rpc_ITransport_s		*transport,
		intptr_t							id,
		tblink_rpc_IParamValMap_t			*result,
		tblink_rpc_IParamValMap_t			*error
		) {
	int32_t ret = 0;
	tblink_rpc_TransportJsonSocket_t *jt = (tblink_rpc_TransportJsonSocket_t *)transport;

	return ret;
}


tblink_rpc_ITransport_t *tblink_rpc_TransportJsonSocket_new(
		int32_t					pid,
		int32_t					sock) {
	tblink_rpc_TransportJsonSocket_t *transport =
			(tblink_rpc_TransportJsonSocket_t *)malloc(sizeof(tblink_rpc_TransportJsonSocket_t));
	memset(transport, 0, sizeof(tblink_rpc_TransportJsonSocket_t));

	transport->pid = pid;
	transport->sock = sock;

	transport->api.dispose = &tblink_rpc_TransportJsonSocket_dispose;
	transport->api.init = &tblink_rpc_TransportJsonSocket_init;
	transport->api.send_req = &tblink_rpc_TransportJsonSocket_send_req;
	transport->api.send_rsp = &tblink_rpc_TransportJsonSocket_send_rsp;

	return &transport->api;
}
