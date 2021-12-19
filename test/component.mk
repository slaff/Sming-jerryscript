COMPONENT_DEPENDS := \
	SmingTest \
	jerryscript

# Jerryscript config
APP_JS_SOURCE_DIR := files
APP_JS_SNAP_UPDATED := touch modules/*
JERRY_GLOBAL_HEAP_SIZE := 8

# Don't need network
HOST_NETWORK_OPTIONS := --nonet
DISABLE_NETWORK := 1

COMPONENT_SRCDIRS := app modules
COMPONENT_INCDIRS := include

# Time in milliseconds to pause after a test group has completed
CONFIG_VARS += TEST_GROUP_INTERVAL
TEST_GROUP_INTERVAL ?= 100
APP_CFLAGS += -DTEST_GROUP_INTERVAL=$(TEST_GROUP_INTERVAL)

# Time in milliseconds to wait before re-starting all tests
# Set to 0 to perform a system restart after all tests have completed
CONFIG_VARS += RESTART_DELAY
ifndef RESTART_DELAY
ifeq ($(SMING_ARCH),Host)
RESTART_DELAY = 0
else
RESTART_DELAY ?= 10000
endif
endif
APP_CFLAGS += -DRESTART_DELAY=$(RESTART_DELAY)

define BuildAndRun
+$(MAKE) snap-clean
+$(MAKE) MODULE="$(patsubst %/test,%/,$(MODULE))$1.$3" JERRY_COMPACT_PROFILE=$2 JERRY_ENABLE_DEBUG=$4 run
endef

.PHONY: execute
execute:
	$(call BuildAndRun,minimal,1,debug,1)
	$(call BuildAndRun,minimal,1,release,0)
	$(call BuildAndRun,es.next,0,debug,1)
	$(call BuildAndRun,es.next,0,release,0)
