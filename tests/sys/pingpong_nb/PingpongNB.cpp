/*
 * PingpongNB.cpp
 *
 *  Created on: Oct 25, 2021
 *      Author: mballance
 */

#include <string>
#include <vector>
#include "PingpongNB.h"

PingpongNB::PingpongNB() {
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

//	ep->init(ep_services, 0);

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
}

