/*
 * tblink_rpc_transport_json_socket.h
 *
 *  Created on: Sep 22, 2021
 *      Author: mballance
 */

#ifndef INCLUDED_TBLINK_RPC_TRANSPORT_JSON_SOCKET_H
#define INCLUDED_TBLINK_RPC_TRANSPORT_JSON_SOCKET_H
#include "tblink_rpc/tblink_rpc.h"


tblink_rpc_ITransport_t *tblink_rpc_TransportJsonSocket_new(
		int32_t					pid,
		int32_t					sock);


#endif /* INCLUDED_TBLINK_RPC_TRANSPORT_JSON_SOCKET_H */

