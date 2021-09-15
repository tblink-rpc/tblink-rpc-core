/*
 * TblinkRpcTestBase.h
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */

#include "gtest/gtest.h"
#include "tblink_rpc/tblink_rpc.h"

class TblinkRpcTestBase : public ::testing::Test {
public:
	TblinkRpcTestBase();

	virtual ~TblinkRpcTestBase();

	virtual void SetUp() override;

	virtual void TearDown() override;

	std::pair<int32_t,uint16_t> create_server();

	int32_t accept_connection(int32_t fd);

	void start_pytest(
			const std::string		&test,
			uint16_t				port);

private:
	int32_t						m_srv_fd;
	int32_t						m_conn_fd;
	pid_t						m_pid;


};

