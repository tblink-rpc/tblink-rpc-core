/*
 * PingpongNB.cpp
 *
 *  Created on: Oct 25, 2021
 *      Author: mballance
 */

#include <string>
#include <vector>
#include "PingpongNB.h"
#include "pingpong_if.h"

PingpongNB::PingpongNB() : m_tbl(0) {
	// TODO Auto-generated constructor stub

}

PingpongNB::~PingpongNB() {
	// TODO Auto-generated destructor stub
}

extern "C" ITbLink *tblink();

void PingpongNB::SetUp() {
	m_tbl = tblink();

	ASSERT_TRUE(m_tbl);
}

TEST_F(PingpongNB, launcher) {
	std::vector<std::string> argv = ::testing::internal::GetArgvs();
	ILaunchType *launch = m_tbl->findLaunchType("process.socket");
	ASSERT_TRUE(launch);

	ILaunchParams *params = m_tbl->newLaunchParams();
	params->add_arg(argv.at(0));
	params->add_arg("--gtest_filter=PingpongNB.remote");

	ILaunchType::result_t res = launch->launch(params);
	if (!res.first) {
		fprintf(stdout, "Error: %s\n", res.second.c_str());
		fflush(stdout);
	}
	ASSERT_TRUE(res.first);

	IEndpoint *ep = res.first;

	ep->init(this, 0);

	int ret;

	while ((ret=ep->is_init()) == 0) {
		if ((ret=ep->process_one_message()) == -1) {
			break;
		}
	}

	ASSERT_EQ(ret, 1);

	IInterfaceType *iftype = pingpong_if::register_type(ep);
	pingpong_if *pingpong_api = new pingpong_if();
	pingpong_api->defineInterfaceInst(ep, "pingpoing", false);



	ASSERT_EQ(ep->build_complete(), 0);
	while ((ret=ep->is_build_complete()) == 0) {
		if ((ret=ep->process_one_message()) == -1) {
			break;
		}
	}

	ASSERT_EQ(ret, 1);

	ASSERT_EQ(ep->connect_complete(), 0);
	while ((ret=ep->is_connect_complete()) == 0) {
		if ((ret=ep->process_one_message()) == -1) {
			break;
		}
	}

	ASSERT_EQ(ret, 1);

	volatile bool rsp_received = false;
	pingpong_api->set_rsp_impl([&](uint32_t v) {
			/*
		fprintf(stdout, "--> launcher: rsp %d\n", v);
		fflush(stdout);
		 */
		rsp_received = true;
		/*
		fprintf(stdout, "<-- launcher: rsp %d\n", v);
		fflush(stdout);
		 */
		return v+1;
	});

	for (uint32_t i=0; i<100000; i++) {
		/*
		fprintf(stdout, "--> launcher: req\n");
		fflush(stdout);
		 */
		uint32_t rv = pingpong_api->req(i);
		ASSERT_EQ(rv, i+1);
		/*
		fprintf(stdout, "<-- launcher: req\n");
		fflush(stdout);
		 */

		// wait for a response
		ret = 0;
		while (!rsp_received) {
			if ((ret=ep->process_one_message()) == -1) {
				break;
			}
		}
		ASSERT_NE(ret, -1);
		rsp_received = false;
	}

}

TEST_F(PingpongNB, remote) {
	fprintf(stdout, "remote\n");
	fflush(stdout);

	ILaunchType *launch = m_tbl->findLaunchType("connect.socket");
	ASSERT_TRUE(launch);

	ILaunchParams *params = m_tbl->newLaunchParams();
	params->add_param("host", getenv("TBLINK_HOST"));
	params->add_param("port", getenv("TBLINK_PORT"));

	ILaunchType::result_t res = launch->launch(params);
	if (!res.first) {
		fprintf(stdout, "Error: %s\n", res.second.c_str());
		fflush(stdout);
	}
	ASSERT_TRUE(res.first);

	IEndpoint *ep = res.first;

	ep->init(this, 0);

	int ret;

	while ((ret=ep->is_init()) == 0) {
		if ((ret=ep->process_one_message()) == -1) {
			break;
		}
	}

	ASSERT_EQ(ret, 1);

	IInterfaceType *iftype = pingpong_if::register_type(ep);
	pingpong_if *pingpong_api = new pingpong_if();
	pingpong_api->defineInterfaceInst(ep, "pingpoing", true);

	ASSERT_EQ(ep->build_complete(), 0);
	while ((ret=ep->is_build_complete()) == 0) {
		if ((ret=ep->process_one_message()) == -1) {
			break;
		}
	}

	ASSERT_EQ(ret, 1);

	ASSERT_EQ(ep->connect_complete(), 0);
	while ((ret=ep->is_connect_complete()) == 0) {
		if ((ret=ep->process_one_message()) == -1) {
			break;
		}
	}

	ASSERT_EQ(ret, 1);

	volatile bool req_received = false;
	pingpong_api->set_req_impl([&](uint32_t v) {
			/*
		fprintf(stdout, "--> remote: req %d\n", v);
		fflush(stdout);
		 */
		req_received = true;
		/*
		fprintf(stdout, "<-- remote: req %d\n", v);
		fflush(stdout);
		 */
		return v+1;
	});

	for (uint32_t i=0; i<100000; i++) {
		// wait for a response
		ret = 0;
		/*
		fprintf(stdout, "--> remote: wait for message\n");
		fflush(stdout);
		 */
		while (!req_received) {
			if ((ret=ep->process_one_message()) == -1) {
				break;
			}
		}
		/*
		fprintf(stdout, "<-- remote: wait for message %d\n", ret);
		fflush(stdout);
		 */

		ASSERT_NE(ret, -1);
		req_received = false;

		uint32_t rv = pingpong_api->rsp(i);
		ASSERT_EQ(rv, i+1);
	}

}

