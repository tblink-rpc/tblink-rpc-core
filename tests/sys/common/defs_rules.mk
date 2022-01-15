SYS_TESTS_COMMONDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
TBLINK_RPC_CORE_DIR := $(abspath $(SYS_TESTS_COMMONDIR)/../../..)
PACKAGES_DIR := $(TBLINK_RPC_CORE_DIR)/packages
DV_MK:=$(shell PATH=$(PACKAGES_DIR)/python/bin:$(PATH) python3 -m mkdv mkfile)

ifneq (1,$(RULES))

MKDV_PYTHONPATH += $(TBLINK_RPC_CORE_DIR)/python

include $(DV_MK)
else # Rules
include $(DV_MK)

endif

