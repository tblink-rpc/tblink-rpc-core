/*
 * tblink_rpc_ITransport.h
 *
 *  Created on: Sep 22, 2021
 *      Author: mballance
 */

#ifndef INCLUDED_TBLINK_RPC_ITRANSPORT_H
#define INCLUDED_TBLINK_RPC_ITRANSPORT_H
#include "tblink_rpc/tblink_rpc_IParamVal.h"

typedef int32_t (*tblink_rpc_recv_req_f)(
		void							*user_data,
		const char						*method,
		intptr_t						id,
		tblink_rpc_IParamValMap_t		*params);

typedef int32_t (*tblink_rpc_recv_rsp_f)(
		void							*user_data,
		intptr_t						id,
		tblink_rpc_IParamValMap_t		*result,
		tblink_rpc_IParamValMap_t		*error);

/********************************************************************
 * ITransport
 ********************************************************************/

typedef struct tblink_rpc_ITransport_s {
	void (*dispose)(
			struct tblink_rpc_ITransport_s		*transport);

	void (*init)(
			struct tblink_rpc_ITransport_s		*transport,
			tblink_rpc_recv_req_f				*req_f,
			tblink_rpc_recv_rsp_f				*rsp_f,
			void								*user_data
			);

	int32_t (*send_req)(
			struct tblink_rpc_ITransport_s		*transport,
			const char							*method,
			intptr_t							id,
			tblink_rpc_IParamValMap_t			*params
			);

	int32_t (*send_rsp)(
			struct tblink_rpc_ITransport_s		*transport,
			const char							*method,
			intptr_t							id,
			tblink_rpc_IParamValMap_t			*params
			);

	int32_t (*process_one_message)(
			struct tblink_rpc_ITransport_s		*transport
			);

	tblink_rpc_IParamValMap_t *(*mkValMap)(
			struct tblink_rpc_ITransport_s		*transport
			);

} tblink_rpc_ITransport_t;




#endif /* INCLUDED_TBLINK_RPC_ITRANSPORT_H */

