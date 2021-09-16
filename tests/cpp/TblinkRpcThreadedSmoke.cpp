/*
 * TblinkRpcThreadedSmoke.cpp
 *
 *  Created on: Sep 16, 2021
 *      Author: mballance
 */

#include "BfmReqRsp.h"
#include "EndpointMsgTransport.h"
#include "TblinkRpcThreadedSmoke.h"
#include "TransportMessageFifo.h"
#include "TransportMessageFifoThreadedEP.h"

using namespace tblink_rpc_core;

TblinkRpcThreadedSmoke::TblinkRpcThreadedSmoke() {
	// TODO Auto-generated constructor stub

}

TblinkRpcThreadedSmoke::~TblinkRpcThreadedSmoke() {
	// TODO Auto-generated destructor stub
}

static void *smoke_thread_main(void *ep_p) {
	IEndpoint *ep = reinterpret_cast<IEndpoint *>(ep_p);

	return 0;
}

TEST_F(TblinkRpcThreadedSmoke, smoke) {

	TransportMessageFifo *transport = new TransportMessageFifo(
			new TransportMessageFifoThreadedEP(),
			new TransportMessageFifoThreadedEP()
			);

	IEndpoint *ep_thread = new EndpointMsgTransport();


}
