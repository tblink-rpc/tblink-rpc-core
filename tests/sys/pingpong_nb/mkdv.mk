MKDV_MK:=$(abspath $(lastword $(MAKEFILE_LIST)))
TEST_DIR:=$(dir $(MKDV_MK))
MKDV_TOOL ?= none


MKDV_RUN_DEPS += run-test

include $(TEST_DIR)/../common/defs_rules.mk

RULES := 1

include $(TEST_DIR)/../common/defs_rules.mk

test-main : $(TEST_DIR)/PingpongNB.cpp pingpong_if.cpp
	$(CXX) -o $@ -g \
		-I. -I$(TBLINK_RPC_CORE_DIR)/cpp/include \
		-I$(TBLINK_RPC_CORE_DIR)/build/googletest/include \
		pingpong_if.cpp $(TEST_DIR)/PingpongNB.cpp  \
		-Wl,--whole-archive \
		$(TBLINK_RPC_CORE_DIR)/build/libtblink_rpc_core_static.a \
		-Wl,--no-whole-archive \
		$(TBLINK_RPC_CORE_DIR)/build/googletest/lib*/libgtest_main*.a \
		$(TBLINK_RPC_CORE_DIR)/build/googletest/lib*/libgtest*.a \
		-ldl -lpthread


remote : $(TEST_DIR)/remote.cpp pingpong_if.cpp
	$(CXX) -o $@ -g \
		-I. -I$(TBLINK_RPC_CORE_DIR)/cpp/include \
		pingpong_if.cpp $(TEST_DIR)/remote.cpp \
		-Wl,--whole-archive \
		$(TBLINK_RPC_CORE_DIR)/build/libtblink_rpc_core_static.a \
		-Wl,--no-whole-archive \
		-ldl


remote : pingpong_if.h

pingpong_if.cpp pingpong_if.h : $(TEST_DIR)/pingpong_if.yaml
	$(PACKAGES_DIR)/python/bin/python3 -m tblink_rpc gen \
		-o pingpong_if.cpp --output-style cpp \
		$^
	$(CXX) -c pingpong_if.cpp -I. -I$(TBLINK_RPC_CORE_DIR)/cpp/include

run-test : test-main
	valgrind --tool=memcheck $(MKDV_RUNDIR)/test-main --gtest_filter=PingpongNB.launcher

