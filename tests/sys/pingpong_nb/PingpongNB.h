/*
 * PingpongNB.h
 *
 *  Created on: Oct 25, 2021
 *      Author: mballance
 */

#pragma once
#include "gtest/gtest.h"
#include "tblink_rpc/ITbLink.h"
#include "tblink_rpc/ILaunchType.h"

using namespace tblink_rpc_core;

class PingpongNB : public ::testing::Test {
public:
	PingpongNB();

	virtual ~PingpongNB();

	virtual void SetUp() override;

protected:
	ITbLink				*m_tbl;

};

