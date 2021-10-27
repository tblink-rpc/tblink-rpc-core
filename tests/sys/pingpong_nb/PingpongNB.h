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
#include "tblink_rpc/IEndpointServices.h"

using namespace tblink_rpc_core;

class PingpongNB : public ::testing::Test, public virtual IEndpointServices {
public:
	PingpongNB();

	virtual ~PingpongNB();

	virtual void SetUp() override;

	virtual void init(IEndpoint *endpoint) override { }

	/**
	 * Return command-line arguments.
	 */
	virtual std::vector<std::string> args() override {
		return {};
	}

	virtual void shutdown() override { }

	virtual int32_t add_time_cb(
			uint64_t 		time,
			intptr_t		callback_id) override {
		return -1;
	}

	virtual void cancel_callback(intptr_t id) override {

	}

	virtual uint64_t time() override {
		return 0;
	}

	/**
	 * Returns the time precision as an exponent:
	 * 0: 1s
	 * -3: 1ms
	 * -6: 1us
	 * -9: 1ns
	 * -12: 1ps
	 */
	virtual int32_t time_precision() override {
		return -1;
	}

	// Release the environment to run
	virtual void run_until_event() override { }

	// Notify that we've hit an event
	virtual void hit_event() override { }

	virtual void idle() override { }

protected:
	ITbLink				*m_tbl;

};

