# JerryScript 
COMPONENT_SUBMODULES := jerryscript

JERRYSCRIPT_ROOT := $(COMPONENT_PATH)/jerryscript

COMPONENT_SRCDIRS := \
	src \
	$(call ListAllSubDirs,$(JERRYSCRIPT_ROOT)/jerry-core)

COMPONENT_INCDIRS := \
	src/include \
	$(JERRYSCRIPT_ROOT)/jerry-core \
	$(COMPONENT_SRCDIRS)

COMPONENT_DOCFILES := jerryscript/docs/img/engines_high_level_design.png
COMPONENT_DOXYGEN_INPUT := src/include

# The size of the JerryScript engine heap. The size in kilobytes is allocated once and defined at compile time.
COMPONENT_VARS := JERRY_GLOBAL_HEAP_SIZE
JERRY_GLOBAL_HEAP_SIZE ?= 1

# Compact (minimal profile) compilation profile makes the JerryScript library smaller
COMPONENT_VARS += JERRY_COMPACT_PROFILE
JERRY_COMPACT_PROFILE ?= 1

ifeq ($(JERRY_COMPACT_PROFILE),1)
JERRY_PROFILE := minimal
# Apply these flags to library and tool(s)
JERRY_COMPILER_FLAGS := \
	JERRY_BUILTINS=0 \
	JERRY_ESNEXT=0 \
	JERRY_UNICODE_CASE_CONVERSION=0
else
JERRY_PROFILE := es.next
endif

JERRY_WEB_COMPILER := $(COMPONENT_PATH)/jsc/$(JERRY_PROFILE)

COMPONENT_CFLAGS += \
	-Wno-error=unused-function \
	-DJERRY_GLOBAL_HEAP_SIZE=$(JERRY_GLOBAL_HEAP_SIZE) \
	-DJERRY_LCACHE=0  \
	-DJERRY_PARSER=0 \
	-DJERRY_SNAPSHOT_EXEC=1 \
	$(addprefix -D,$(JERRY_COMPILER_FLAGS))

ifndef SMING_RELEASE
	COMPONENT_CFLAGS += \
		-DJERRY_NDEBUG \
		-DJERRY_LOGGING=1 \
		-DJERRY_MEM_STATS=1
endif

# Build version of tool compatible with library
DEBUG_VARS += JERRY_SNAPSHOT_TOOL
JERRY_BUILD_DIR := $(COMPONENT_PATH)/jerryscript/out/$(call CalculateVariantHash,JERRY_COMPILER_FLAGS)
JERRY_SNAPSHOT_TOOL := $(JERRY_BUILD_DIR)/bin/jerry-snapshot$(TOOL_EXT)

ifeq ($(UNAME),Windows)
JERRY_CMAKE_PARAMS := \
	--cmake-param "-GMSYS Makefiles" \
	--compile-flag "-I $(JERRYSCRIPT_ROOT)/../src/include" \
	--compile-flag "-Wno-error=unused-parameter " \
	--compile-flag "-D_POSIX_C_SOURCE=1 -U__STRICT_ANSI__"
endif

$(JERRY_SNAPSHOT_TOOL):
	$(Q) $(PYTHON) $(JERRYSCRIPT_ROOT)/tools/build.py \
		--lto OFF \
		--jerry-cmdline-snapshot ON \
		--builddir "$(JERRY_BUILD_DIR)" \
		$(JERRY_CMAKE_PARAMS) \
		$(patsubst %,--compile-flag "-D %",$(JERRY_COMPILER_FLAGS))

jerryscript-clean: jerry-tools-clean

.PHONY: jerry-tools-clean
jerry-tools-clean:
	$(Q) rm -rf $(abspath $(JERRY_BUILD_DIR)/..)

# Support for building .snap files from .js source

DEBUG_VARS += APP_JS_SOURCE_DIR APP_JS_SNAP_DIR

ifdef APP_JS_SOURCE_DIR
APP_JS_SNAP_DIR ?= out/jerryscript

APP_JS_SOURCE_FILES := $(notdir $(wildcard $(APP_JS_SOURCE_DIR)/*.js))
APP_JS_SNAP_FILES := $(patsubst %,$(APP_JS_SNAP_DIR)/%.snap,$(APP_JS_SOURCE_FILES))

COMPONENT_PREREQUISITES := $(APP_JS_SNAP_FILES)

$(APP_JS_SNAP_FILES): $(JERRY_SNAPSHOT_TOOL) $(addprefix $(APP_JS_SOURCE_DIR)/,$(APP_JS_SOURCE_FILES)) | $(APP_JS_SNAP_DIR)
	$(Q) $(JERRY_SNAPSHOT_TOOL) generate $(APP_JS_SOURCE_DIR)/$(@F:.snap=) -o $@
	$(Q) $(APP_JS_SNAP_UPDATED)

$(APP_JS_SNAP_DIR):
	mkdir -p $@

clean: snap-clean
snap-clean:
	$(Q) rm -rf $(APP_JS_SNAP_DIR)
endif
