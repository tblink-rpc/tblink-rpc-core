/*
 * LaunchTypeConnectSocket.cpp
 *
 *  Created on: Sep 27, 2021
 *      Author: mballance
 */

#include <string.h>
#include <stdio.h>
#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
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

#include "EndpointMsgTransport.h"
#include "LaunchParams.h"
#include "LaunchTypeConnectSocket.h"
#include "LaunchTypeRegistration.h"
#include "TransportJsonSocket.h"

namespace tblink_rpc_core {

LaunchTypeConnectSocket::LaunchTypeConnectSocket() {
	// TODO Auto-generated constructor stub

}

LaunchTypeConnectSocket::~LaunchTypeConnectSocket() {
	// TODO Auto-generated destructor stub
}

ILaunchType::result_t LaunchTypeConnectSocket::launch(ILaunchParams *params) {

	// TODO: need to handle non-localhost hosts
	if (params->params().find("host") == params->params().end()) {
		delete params;
		return {0, "No 'host' param speicifed"};
	}

	if (params->params().find("port") == params->params().end()) {
		delete params;
		return {0, "No 'port' param speicifed"};
	}

	int32_t port = strtol(params->params().find("port")->second.c_str(), 0, 0);

    int sock = socket (AF_INET, SOCK_STREAM, 0);
     if (sock == -1) {
    	 delete params;
         return {0, "socket creation failed"};
     }

     struct sockaddr_in addr;

     memset(&addr, 0, sizeof(addr));

     addr.sin_family = AF_INET;
     addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
     addr.sin_port = htons(port);

     if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
         fprintf(stdout, "Error: Connection error\n");
         ::close(sock);
         delete params;
         return {0, "connection error"};
     }

     int flags =1;
     ::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flags, sizeof(flags));

     TransportJsonSocket *transport = new TransportJsonSocket(0, sock);
     EndpointMsgTransport *endpoint = new EndpointMsgTransport(transport);

     delete params;

	return {endpoint, ""};
}

ILaunchParams *LaunchTypeConnectSocket::newLaunchParams() {
	return new LaunchParams();
}

static LaunchTypeRegistration<LaunchTypeConnectSocket>	m_registration;

} /* namespace tblink_rpc_core */

