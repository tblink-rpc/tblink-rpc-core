/*
 * TblinkRpcThreadedSmoke.cpp
 *
 *  Created on: Sep 16, 2021
 *      Author: mballance
 */

#ifndef _WIN32
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
#include <fcntl.h>
#else
#include <winsock2.h>
#endif
#include "BfmReqRsp.h"
#include "EndpointMsgTransport.h"
#include "TblinkRpcThreadedSmoke.h"
#include "TransportJsonSocket.h"
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

/********************************************************************
 * nb_pingpong_fifo
 ********************************************************************/

static void *nb_pingpong_fifo_thread_main(void *ep_p) {
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

	pthread_cond_t rsp_cond;
	pthread_mutex_t rsp_mut;

	bool have_rsp = false;
	pthread_cond_init(&rsp_cond, 0);
	pthread_mutex_init(&rsp_mut, 0);

	BfmReqRsp *inst = BfmReqRsp::mkInst(ep, "inst", false);
	inst->rsp_f([&](int32_t v) {
		fprintf(stdout, "Thread: rsp %d\n", v);
		pthread_mutex_lock(&rsp_mut);
		have_rsp = true;
		pthread_cond_signal(&rsp_cond);
		pthread_mutex_unlock(&rsp_mut);
	});

	ep->build_complete();

	fprintf(stdout, "--> Thread: await build_complete\n");
	fflush(stdout);
	while (ep->is_build_complete() == 0) {
		ep->process_one_message();
	}
	fprintf(stdout, "<-- Thread: await build_complete\n");
	fflush(stdout);

	ep->connect_complete();

	fprintf(stdout, "--> Thread: await connect_complete\n");
	fflush(stdout);
	while (ep->is_connect_complete() == 0) {
		ep->process_one_message();
	}
	fprintf(stdout, "<-- Thread: await connect_complete\n");
	fflush(stdout);

	for (uint32_t i=0; i<10; i++) {
		fprintf(stdout, "--> Thread: call req %d\n", i);
		fflush(stdout);
		inst->req(i);
		fprintf(stdout, "<-- Thread: call req %d\n", i);
		fflush(stdout);

		fprintf(stdout, "--> Thread: wait rsp %d\n", i);
		fflush(stdout);

		while (true) {
			bool val = false;
			pthread_mutex_lock(&rsp_mut);

			if (!have_rsp) {
				pthread_cond_wait(&rsp_cond, &rsp_mut);
			}

			val = have_rsp;
			have_rsp = false;

			pthread_mutex_unlock(&rsp_mut);

			if (val) {
				break;
			}
		}

		fprintf(stdout, "<--> Thread: wait rsp %d\n", i);
		fflush(stdout);
	}

	return 0;
}

TEST_F(TblinkRpcThreadedSmoke, nb_pingpong_fifo) {

	TransportMessageFifo *transport = new TransportMessageFifo(
			new TransportMessageFifoThreadedEP(),
			new TransportMessageFifoThreadedEP()
			);

	IEndpoint *ep_thread = new EndpointMsgTransport(transport->ep(0));
	IEndpoint *ep_main = new EndpointMsgTransport(transport->ep(1));
	TestEPServices services;
	pthread_t thread_h;

	pthread_create(&thread_h, 0, &nb_pingpong_fifo_thread_main,
			reinterpret_cast<void *>(ep_thread));

	ep_main->init(&services, 0);

	BfmReqRsp::registerType(ep_main);

	while (ep_main->is_init() == 0) {
		ep_main->process_one_message();
	}

	bool have_req = false;
	pthread_cond_t req_cond;
	pthread_mutex_t req_mut;

	pthread_cond_init(&req_cond, 0);
	pthread_mutex_init(&req_mut, 0);

	BfmReqRsp *inst = BfmReqRsp::mkInst(ep_main, "inst", true);
	inst->req_f([&](int32_t v) {
		fprintf(stdout, "Main: req: %d\n", v);
		pthread_mutex_lock(&req_mut);
		have_req = true;
		pthread_cond_signal(&req_cond);
		pthread_mutex_unlock(&req_mut);
	});

	ep_main->build_complete();

	fprintf(stdout, "--> Main: await build_complete\n");
	fflush(stdout);
	while (ep_main->is_build_complete() == 0) {
		ep_main->process_one_message();
	}
	fprintf(stdout, "<-- Main: await build_complete\n");
	fflush(stdout);

	ep_main->connect_complete();

	fprintf(stdout, "--> Main: await connect_complete\n");
	fflush(stdout);
	while (ep_main->is_connect_complete() == 0) {
		ep_main->process_one_message();
	}
	fprintf(stdout, "<-- Main: await connect_complete\n");
	fflush(stdout);

	for (uint32_t i=0; i<10; i++) {
		fprintf(stdout, "--> Main: Wait req\n");

		while (true) {
			bool req = false;

			pthread_mutex_lock(&req_mut);

			if (!have_req) {
				pthread_cond_wait(&req_cond, &req_mut);
			}
			req = have_req;
			have_req = false;

			pthread_mutex_unlock(&req_mut);

			if (req) {
				break;
			}
		}

		inst->rsp(i+1);

		fprintf(stdout, "<-- Main: Wait req\n");
	}

	fprintf(stdout, "--> pthread_join\n");
	pthread_join(thread_h, 0);
	fprintf(stdout, "<-- pthread_join\n");

}

/********************************************************************
 * nb_pingpong_fifo
 ********************************************************************/

static void *nb_pingpong_socket_thread_main(void *port_v) {
	intptr_t port = reinterpret_cast<intptr_t>(port_v);

	fprintf(stdout, "Thread: port=%lld\n", port);

	int sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
	    fprintf(stdout, "Error: Socket creation error");
	    return 0;
	}

	struct sockaddr_in addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);

	if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
	    fprintf(stdout, "Error: Connection error\n");
	    close(sock);
	    return 0;
	}

	int flags =1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flags, sizeof(flags));

	TransportJsonSocket *transport = new TransportJsonSocket(0, sock);
	EndpointMsgTransport *ep = new EndpointMsgTransport(transport);

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

	pthread_cond_t rsp_cond;
	pthread_mutex_t rsp_mut;

	bool have_rsp = false;
	pthread_cond_init(&rsp_cond, 0);
	pthread_mutex_init(&rsp_mut, 0);

	BfmReqRsp *inst = BfmReqRsp::mkInst(ep, "inst", false);
	inst->rsp_f([&](int32_t v) {
		fprintf(stdout, "Thread: rsp %d\n", v);
		pthread_mutex_lock(&rsp_mut);
		have_rsp = true;
		pthread_cond_signal(&rsp_cond);
		pthread_mutex_unlock(&rsp_mut);
	});

	ep->build_complete();

	fprintf(stdout, "--> Thread: await build_complete\n");
	fflush(stdout);
	while (ep->is_build_complete() == 0) {
		ep->process_one_message();
	}
	fprintf(stdout, "<-- Thread: await build_complete\n");
	fflush(stdout);

	ep->connect_complete();

	fprintf(stdout, "--> Thread: await connect_complete\n");
	fflush(stdout);
	while (ep->is_connect_complete() == 0) {
		ep->process_one_message();
	}
	fprintf(stdout, "<-- Thread: await connect_complete\n");
	fflush(stdout);

	for (uint32_t i=0; i<10; i++) {
		fprintf(stdout, "--> Thread: call req %d\n", i);
		fflush(stdout);
		inst->req(i);
		fprintf(stdout, "<-- Thread: call req %d\n", i);
		fflush(stdout);

		fprintf(stdout, "--> Thread: wait rsp %d\n", i);
		fflush(stdout);

		while (!have_rsp) {
			ep->process_one_message();
		}
		have_rsp = false;

		/*
		while (true) {
			bool val = false;
			pthread_mutex_lock(&rsp_mut);

			if (!have_rsp) {
				pthread_cond_wait(&rsp_cond, &rsp_mut);
			}

			val = have_rsp;
			have_rsp = false;

			pthread_mutex_unlock(&rsp_mut);

			if (val) {
				break;
			}
		}
		 */

		fprintf(stdout, "<--> Thread: wait rsp %d\n", i);
		fflush(stdout);
	}

	return 0;
}

TEST_F(TblinkRpcThreadedSmoke, nb_pingpong_socket) {

	/*
	TransportMessageFifo *transport = new TransportMessageFifo(
			new TransportMessageFifoThreadedEP(),
			new TransportMessageFifoThreadedEP()
			);
	 */

//	IEndpoint *ep_thread = new EndpointMsgTransport(transport->ep(0));
	pthread_t thread_h;

	struct sockaddr_in serv_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_addr.sin_port = 0;

    if ((bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
    	perror("Error binding");
    }

    socklen_t size = sizeof(serv_addr);
    getsockname(sockfd, (struct sockaddr *) &serv_addr, &size);

    ::listen(sockfd, 1);

    uint32_t port = ntohs(serv_addr.sin_port);

    fprintf(stdout, "port: %d\n", port);

	pthread_create(&thread_h, 0, &nb_pingpong_socket_thread_main,
			reinterpret_cast<void *>(port));

	fprintf(stdout, "--> accept\n");
	// Now, accept a connection from the thread
	unsigned int clilen = sizeof(serv_addr);
	int conn_fd = accept(sockfd, (struct sockaddr *)&serv_addr, &clilen);
	fprintf(stdout, "<-- accept\n");

	int flags =1;
	setsockopt(conn_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flags, sizeof(flags));

	fprintf(stdout, "conn_fd=%d\n", conn_fd);

	TransportJsonSocket *transport = new TransportJsonSocket(0, conn_fd);
	IEndpoint *ep_main = new EndpointMsgTransport(transport);
	TestEPServices services;

	ep_main->init(&services, 0);

	BfmReqRsp::registerType(ep_main);

	while (ep_main->is_init() == 0) {
		ep_main->process_one_message();
	}

	bool have_req = false;
	pthread_cond_t req_cond;
	pthread_mutex_t req_mut;

	pthread_cond_init(&req_cond, 0);
	pthread_mutex_init(&req_mut, 0);

	BfmReqRsp *inst = BfmReqRsp::mkInst(ep_main, "inst", true);
	inst->req_f([&](int32_t v) {
		fprintf(stdout, "Main: req: %d\n", v);
		pthread_mutex_lock(&req_mut);
		have_req = true;
		pthread_cond_signal(&req_cond);
		pthread_mutex_unlock(&req_mut);
	});

	ep_main->build_complete();

	fprintf(stdout, "--> Main: await build_complete\n");
	fflush(stdout);
	while (ep_main->is_build_complete() == 0) {
		ep_main->process_one_message();
	}
	fprintf(stdout, "<-- Main: await build_complete\n");
	fflush(stdout);

	ep_main->connect_complete();

	fprintf(stdout, "--> Main: await connect_complete\n");
	fflush(stdout);
	while (ep_main->is_connect_complete() == 0) {
		ep_main->process_one_message();
	}
	fprintf(stdout, "<-- Main: await connect_complete\n");
	fflush(stdout);

	for (uint32_t i=0; i<10; i++) {
		fprintf(stdout, "--> Main: Wait req\n");

		while (!have_req) {
			ep_main->process_one_message();
		}
		have_req = false;
		/*
		while (true) {
			bool req = false;

			pthread_mutex_lock(&req_mut);

			if (!have_req) {
				pthread_cond_wait(&req_cond, &req_mut);
			}
			req = have_req;
			have_req = false;

			pthread_mutex_unlock(&req_mut);

			if (req) {
				break;
			}
		}
		 */

		inst->rsp(i+1);

		fprintf(stdout, "<-- Main: Wait req\n");
	}

	fprintf(stdout, "--> pthread_join\n");
	pthread_join(thread_h, 0);
	fprintf(stdout, "<-- pthread_join\n");

}

