/*
 * Debug.h
 *
 *  Created on: Oct 26, 2021
 *      Author: mballance
 */

#pragma once

#define DEBUG_FLUSH

#define DEBUG_SCOPE(scope)
#ifdef DEBUG_FLUSH
#define DEBUG_ENTER_BASE(scope, fmt, ...) \
	fprintf(stdout, "--> " #scope "::" fmt "\n", ##__VA_ARGS__); fflush(stdout)
#define DEBUG_LEAVE_BASE(scope, fmt, ...) \
	fprintf(stdout, "<-- " #scope "::" fmt "\n", ##__VA_ARGS__); fflush(stdout)
#define DEBUG_BASE(scope, fmt, ...) \
	fprintf(stdout, #scope ": " fmt "\n", ##__VA_ARGS__); fflush(stdout)
#else
#define DEBUG_ENTER_BASE(scope, fmt, ...) \
	fprintf(stdout, "--> " #scope "::" fmt "\n", ##__VA_ARGS__)
#define DEBUG_LEAVE_BASE(scope, fmt, ...) \
	fprintf(stdout, "<-- " #scope "::" fmt "\n", ##__VA_ARGS__)
#define DEBUG_BASE(scope, fmt, ...) \
	fprintf(stdout, #scope ": " fmt "\n", ##__VA_ARGS__)
#endif

namespace tblink_rpc_core {

class Debug {
public:
	Debug();
	virtual ~Debug();
};

} /* namespace tblink_rpc_core */

