/*
 * TblinkRpcTestBase.cpp
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */

#include "TblinkRpcTestBase.h"
#ifndef _WIN32
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif

#include <spawn.h>
#include <unistd.h>
#include <fcntl.h>
#include <alloca.h>
#include "Env.h"
#include "Path.h"



TblinkRpcTestBase::TblinkRpcTestBase() {
	m_srv_fd = -1;
	m_conn_fd = -1;
	m_pid = 0;
	// TODO Auto-generated constructor stub

}

TblinkRpcTestBase::~TblinkRpcTestBase() {
	// TODO Auto-generated destructor stub
}

void TblinkRpcTestBase::SetUp() {
	;
}

void TblinkRpcTestBase::TearDown() {
	if (m_conn_fd != -1) {
		::close(m_conn_fd);
	}
	if (m_srv_fd != -1) {
		::close(m_srv_fd);
	}

	if (m_pid != 0) {
		int status;
		::waitpid(m_pid, &status, 0);

		if (status != 0) {
			FAIL() << "Python fail";
		}
	}

	m_srv_fd = -1;
	m_conn_fd = -1;
}

std::pair<int32_t, uint16_t> TblinkRpcTestBase::create_server() {
	uint16_t port;

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

    m_srv_fd = sockfd;
    port = ntohs(serv_addr.sin_port);

	return {sockfd, port};
}

int32_t TblinkRpcTestBase::accept_connection(int32_t fd) {
	struct sockaddr_in serv_addr;

	unsigned int clilen = sizeof(serv_addr);
	int conn_fd = accept(fd, (struct sockaddr *)&serv_addr, &clilen);

	m_conn_fd = conn_fd;

	return conn_fd;
}

void TblinkRpcTestBase::start_pytest(
			const std::string		&test,
			uint16_t				port) {
	std::vector<std::string> args;
	char		cwd[1024], tmp[16];

	std::vector<std::string> cmdline = ::testing::internal::GetArgvs();
	std::string test_exec = Path::findpath(cmdline.at(0));

	fprintf(stdout, "test_exec: %s\n", test_exec.c_str());
	std::string basedir = Path::dirname(
			Path::dirname(Path::dirname(test_exec)));
	fprintf(stdout, "basedir: %s\n", basedir.c_str());

	std::string python = Path::join({basedir, "packages", "python", "bin", "python3"});
	fprintf(stdout, "python: %s\n", python.c_str());

	Env env;

	std::string tests = Path::join({basedir, "tests", "python"});
	std::string python_src = Path::join({basedir, "python"});
	std::string pythonpath = tests;
	pythonpath += ":" + python_src;
	pythonpath += ":" + env.get("PYTHONPATH");

	fprintf(stdout, "pythonpath=%s\n", pythonpath.c_str());
	env.set("PYTHONPATH", pythonpath);
	fprintf(stdout, "PYTHONPATH=%s\n", env.get("PYTHONPATH").c_str());

	::getcwd(cwd, sizeof(cwd));

	args.push_back(python);
	args.push_back("-m");
	args.push_back("unittest");
	args.push_back(test);

	sprintf(tmp, "%d", port);
	env.set("TBLINK_PORT", tmp);

    char **argv = (char **)alloca(sizeof(char *)*args.size()+1);
    for (uint32_t i=0; i<args.size(); i++) {
        char *t = (char *)alloca(args.at(i).size()+1);
        strcpy(t, args.at(i).c_str());
        argv[i] = t;
    }
    argv[args.size()] = 0;

    ::chdir(tests.c_str());
    int32_t status = posix_spawnp(
    		&m_pid,
			argv[0],
			0,
			0,
			(char *const *)argv,
			env.envp());
    ::chdir(cwd);
}
