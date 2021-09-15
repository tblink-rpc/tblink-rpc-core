/*
 * TblinkRpcSmoke.cpp
 *
 *  Created on: Jul 4, 2021
 *      Author: mballance
 */

#include "TblinkRpcSmoke.h"
using namespace tblink_rpc_core;

TblinkRpcSmoke::TblinkRpcSmoke() {
	// TODO Auto-generated constructor stub

}

TblinkRpcSmoke::~TblinkRpcSmoke() {
	// TODO Auto-generated destructor stub
}

/*
static void call_f(
		IInterfaceInst		*inst,
		IMethodType			*method,
		intptr_t			call_id,
		IParamValVectorSP	params) {

}
 */

TEST_F(TblinkRpcSmoke, smoke) {
	std::pair<int32_t, uint16_t> srv = create_server();

	fprintf(stdout, "srv: %d,%d\n", srv.first, (int32_t)srv.second);

	// TODO: launch client to connect on port
	start_pytest(
			"tblink_rpc_smoke.TblinkRpcSmoke.test_smoke",
			srv.second);

	int32_t client_fd = accept_connection(srv.first);

	fprintf(stdout, "client_fd: %d\n", client_fd);

	IFactory *f = tblink_rpc_get_factory();
//	ITransport *tp = f->mkSocketTransport(client_fd);
//	IEndpoint *ep = f->mkJsonRpcEndpoint(tp);

//	IInterfaceTypeBuilder *tb = ep->newInterfaceTypeBuilder("my_pkg.my_t");
//	IMethodType *my_method = tb->define_method(
//			"my_method",
//			-1, // auto-assign the ID
//			"v()",
//			false, // Import method
//			false);
//	IInterfaceType *t = ep->defineInterfaceType(tb);
//
//	IInterfaceInst *static_i = ep->defineInterfaceInst(
//			t,
//			"top.foo.bar",
//			call_f);

//	ep->build_complete();

	// TODO: lookup methods from the other side

//	ep->connect_complete();

	// TODO: run

}
