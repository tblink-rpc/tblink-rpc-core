/*
 * TblinkRpcThreadedSmoke.cpp
 *
 *  Created on: Sep 16, 2021
 *      Author: mballance
 */

#include <pthread.h>
#include "BfmReqRsp.h"
#include "EndpointMsgTransport.h"
#include "TblinkRpcThreadedSmoke.h"
#include "TransportMessageFifo.h"
#include "TransportMessageFifoThreadedEP.h"
#include "TestEPServices.h"

using namespace tblink_rpc_core;

TblinkRpcThreadedSmoke::TblinkRpcThreadedSmoke() {
	// TODO Auto-generated constructor stub

}

TblinkRpcThreadedSmoke::~TblinkRpcThreadedSmoke() {
	// TODO Auto-generated destructor stub
}

static void *smoke_thread_main(void *ep_p) {
	IEndpoint *ep = reinterpret_cast<IEndpoint *>(ep_p);
	TestEPServices services;
	fprintf(stdout, "smoke_thread_main\n");
	fflush(stdout);

	ep->init(&services, 0);

	fprintf(stdout, "--> Thread: await init\n");
	fflush(stdout);
	while (ep->is_init() == 0) {
		ep->process_one_message();
	}
	fprintf(stdout, "<-- Thread: await init\n");
	fflush(stdout);

	BfmReqRsp::registerType(ep);

	ep->build_complete();

	fprintf(stdout, "--> Thread: await build_complete\n");
	fflush(stdout);
	while (ep->is_build_complete() == 0) {
		ep->process_one_message();
	}
	fprintf(stdout, "<-- Thread: await build_complete\n");
	fflush(stdout);

	return 0;
}

TEST_F(TblinkRpcThreadedSmoke, smoke) {

	TransportMessageFifo *transport = new TransportMessageFifo(
			new TransportMessageFifoThreadedEP(),
			new TransportMessageFifoThreadedEP()
			);

	IEndpoint *ep_thread = new EndpointMsgTransport(transport->ep(0));
	IEndpoint *ep_main = new EndpointMsgTransport(transport->ep(1));
	TestEPServices services;
	pthread_t thread_h;

	pthread_create(&thread_h, 0, &smoke_thread_main,
			reinterpret_cast<void *>(ep_thread));

	ep_main->init(&services, 0);

	BfmReqRsp::registerType(ep_main);

	while (ep_main->is_init() == 0) {
		ep_main->process_one_message();
	}


	ep_main->build_complete();

	fprintf(stdout, "--> Main: await build_complete\n");
	fflush(stdout);
	while (ep_main->is_build_complete() == 0) {
		ep_main->process_one_message();
	}
	fprintf(stdout, "<-- Main: await build_complete\n");
	fflush(stdout);

}
