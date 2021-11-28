/*
 * LaunchTypeProcessSocket.cpp
 *
 *  Created on: Sep 19, 2021
 *      Author: mballance
 */

#include "EndpointMsgTransport.h"
#include "Env.h"
#include "LaunchParams.h"
#include "TransportJsonSocket.h"
#include "LaunchTypeProcessSocket.h"
#include "LaunchTypeRegistration.h"

#ifndef _WIN32
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
#include <fcntl.h>
static const char PS = ':';
#else
#include <winsock2.h>
static const char PS = ';';
#endif
#include <spawn.h>
#include <string.h>

extern char **environ;


namespace tblink_rpc_core {

LaunchTypeProcessSocket::LaunchTypeProcessSocket() {
	// TODO Auto-generated constructor stub

}

LaunchTypeProcessSocket::~LaunchTypeProcessSocket() {
	// TODO Auto-generated destructor stub
}

ILaunchType::result_t LaunchTypeProcessSocket::launch(ILaunchParams *params) {
	// Create the socket server
	struct sockaddr_in serv_addr;

	int32_t srv_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_addr.sin_port = 0;

    if ((bind(srv_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
    	perror("Error binding");
    }

    socklen_t size = sizeof(serv_addr);
    getsockname(srv_socket, (struct sockaddr *) &serv_addr, &size);

    listen(srv_socket, 1);

   	pid_t pid;
    {
//    	std::vector<std::string> args;
   		Env env;

    	char tmp[16], hostname[16];

//    	args.push_back("valgrind");
//    	args.push_back("--tool=memcheck");

    	sprintf(tmp, "%d", ntohs(serv_addr.sin_port));
    	env.set("TBLINK_PORT", tmp);
    	env.set("TBLINK_HOST", "localhost");

    	std::vector<std::string> args = params->args();

    	if (env.has("TBLINK_GDBSRV") && env.get("TBLINK_GDBSRV") == "1") {
    		args.insert(args.begin(), {"gdbserver", ":6001"});
    	} else if (env.has("TBLINK_GDB") && env.get("TBLINK_GDB") == "1") {
    		args.insert(args.begin(), {"xterm", "-e", "gdb", "--args"});
    	} else if (env.has("TBLINK_VALGRIND") && env.get("TBLINK_VALGRIND") == "1") {
    		args.insert(args.begin(), {"valgrind", "--tool=memcheck"});
    	}

    	fprintf(stdout, "Launch Args: %d\n", args.size());
    	fflush(stdout);

    	char **argv = (char **)alloca(sizeof(char *)*(args.size()+1));

    	for (uint32_t i=0; i<args.size(); i++) {
    		fprintf(stdout, "Launch arg[%d]: %s\n", i, args.at(i).c_str());
    		fflush(stdout);
    		argv[i] = strdup(args.at(i).c_str());
    	}
    	argv[args.size()] = 0;

    	// TODO: need proper propagation of hostname/port
    	int status = posix_spawnp(
    			&pid,
				argv[0],
				0,
				0,
				(char *const *)argv,
				env.environ_p());

    	// Now, clean up the arguments
    	for (uint32_t i=0; i<params->args().size(); i++) {
    		free(argv[i]);
    	}

    	if (status != 0) {
    		return {0, "Failed to launch"};
    	}
    }

	// Wait for a connection, or for
	// the interpreter process to exit
   	int32_t conn_socket = -1;
    {
    	// Wait for a bit to see if we get a connect request
    	fd_set rfds;
    	struct timeval tv;
    	int retval;

    	/*
    	 * Wait for a connect request for 5s at a time.
    	 */
    	for (uint32_t i=0; i<500; i++) {
    		tv.tv_sec = 5;
    		tv.tv_usec = 0;

    		FD_ZERO(&rfds);
    		FD_SET(srv_socket, &rfds);

    		retval = select(srv_socket+1, &rfds, 0, 0, &tv);

    		if (retval > 0) {
    			break;
    		} else {
    			// Check that the process is still alive
    			int status;
    			retval = ::waitpid(pid, &status, WNOHANG);

    			// If the process either doesn't exist
    			// or has terminated, then bail out early
    			if (retval != 0) {
    				return {0, "Remote process has terminated"};
    			}
    		}
    	}

    	if (retval > 0) {
    		unsigned int clilen = sizeof(serv_addr);
           	int flag = 1;
    		conn_socket = accept(srv_socket, (struct sockaddr *)&serv_addr, &clilen);

           	::setsockopt(
           			conn_socket,
					IPPROTO_TCP,
					TCP_NODELAY,
					(char *)&flag,
					sizeof(int));
    	} else {
    		// TODO: shut down the remote?
    		return {0, "Failed to connect within appropriate interval"};
    	}
    }

    TransportJsonSocket *transport = new TransportJsonSocket(pid, conn_socket);
    IEndpoint *ep = new EndpointMsgTransport(transport);

    delete params;

    return {ep, ""};
}

ILaunchParams *LaunchTypeProcessSocket::newLaunchParams() {
	return new LaunchParams();
}

static LaunchTypeRegistration<LaunchTypeProcessSocket>	m_registration;

} /* namespace tblink_rpc_core */
