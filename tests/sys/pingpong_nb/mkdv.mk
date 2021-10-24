MKDV_MK:=$(abspath $(lastword $(MAKEFILE_LIST)))
TEST_DIR:=$(dir $(MKDV_MK))
MKDV_TOOL ?= none


MKDV_BUILD_DEPS += launcher remote

include $(TEST_DIR)/../common/defs_rules.mk

RULES := 1

include $(TEST_DIR)/../common/defs_rules.mk

launcher : pingpong_if.h

remote : pingpong_if.h

pingpong_if.cpp pingpong_if.h : $(TEST_DIR)/pingpong_if.yaml
	$(PACKAGES_DIR)/python/bin/python3 -m tblink_rpc gen \
		-o pingpong_if.cpp --output-style cpp \
		$^

