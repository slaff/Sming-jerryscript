# JerryScript
COMPONENT_SUBMODULES := jerryscript

JERRYSCRIPT_ROOT := $(COMPONENT_PATH)/jerryscript
JERRYSCRIPT_SRCDIRS := $(call ListAllSubDirs,$(JERRYSCRIPT_ROOT)/jerry-core)

COMPONENT_SRCDIRS := \
	src \
	$(JERRYSCRIPT_SRCDIRS)

COMPONENT_INCDIRS := \
	src/include \
	$(JERRYSCRIPT_ROOT)

EXTRA_INCDIR := \
	$(JERRYSCRIPT_ROOT)/jerry-core \
	$(JERRYSCRIPT_SRCDIRS)

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
GLOBAL_CFLAGS += -DJERRY_ESNEXT=1
endif

JERRY_WEB_COMPILER := $(COMPONENT_PATH)/jsc/$(JERRY_PROFILE)

COMPONENT_CFLAGS += \
	-Wformat=0 \
	-DJERRY_LCACHE=0  \
	-DJERRY_SNAPSHOT_EXEC=1 \
	$(addprefix -D,$(JERRY_COMPILER_FLAGS))

COMPONENT_CPPFLAGS += \
	-DJERRY_GLOBAL_HEAP_SIZE=$(JERRY_GLOBAL_HEAP_SIZE)

#
COMPONENT_VARS += JERRY_ENABLE_DEBUG
JERRY_ENABLE_DEBUG ?= 0
ifeq ($(JERRY_ENABLE_DEBUG),1)
  COMPONENT_CFLAGS += -DJERRY_LOGGING=1
  GLOBAL_CFLAGS += -DJERRY_ENABLE_DEBUG=1
else
  COMPONENT_CFLAGS += -DJERRY_NDEBUG=1
endif

#
COMPONENT_RELINK_VARS += JERRY_MEM_STATS
JERRY_MEM_STATS ?= 1
COMPONENT_CFLAGS += -DJERRY_MEM_STATS=$(JERRY_MEM_STATS)

#
COMPONENT_RELINK_VARS += JERRY_ERROR_MESSAGES
JERRY_ERROR_MESSAGES ?= 0
COMPONENT_CFLAGS += -DJERRY_ERROR_MESSAGES=$(JERRY_ERROR_MESSAGES)

#
COMPONENT_RELINK_VARS += JERRY_PARSER
JERRY_PARSER ?= 0
COMPONENT_CFLAGS += -DJERRY_PARSER=$(JERRY_PARSER)

#
COMPONENT_VARS += JERRY_EXTERNAL_CONTEXT
JERRY_EXTERNAL_CONTEXT ?= 0
ifeq ($(JERRY_EXTERNAL_CONTEXT),1)
GLOBAL_CFLAGS += -DJERRY_EXTERNAL_CONTEXT=1
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
	$(Q) CC= $(PYTHON) $(JERRYSCRIPT_ROOT)/tools/build.py \
		--lto OFF \
		--jerry-cmdline-snapshot ON \
		--builddir "$(JERRY_BUILD_DIR)" \
		--error-messages ON \
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
	$(Q) mkdir -p $@

clean: snap-clean
snap-clean:
	$(Q) rm -rf $(APP_JS_SNAP_DIR)
endif

# We want maps for some jerryscript types to allow strong type aliases and printability
# Generate these by parsing jerryscript headers

JERRY_TYPES_H := $(COMPONENT_PATH)/src/include/Jerryscript/.typemaps.h

ifeq ($(UNAME),Darwin)
SED ?= gsed
else
SED ?= sed
endif

# Generate MAP #define for a jerryscript C enumeration
# $1 -> e.g. JERRY_FUNCTION_TYPE_
# $2 -> source file
# $3 -> optional name for map
define JerryGetTypes
$(Q) printf "#define $(if $3,$3_,$1)MAP(XX)" >> $@
$(Q) $(foreach v,\
$(shell $(AWK) -F " |,|=" '/$1/ { sub(/^ +$1/,""); print $$1 }' $(JERRYSCRIPT_ROOT)/jerry-core/include/jerryscript-$2.h),\
printf " \\\\\n\tXX($1$v, %s)" $$( echo "$v" | $(SED) -E 's/(.*)/\L\1/; s/(^|_)([a-z0-9])/\U\2/g' ) >> $@; )
$(Q) printf "\n\n" >> $@
endef

COMPONENT_PREREQUISITES += $(JERRY_TYPES_H)

$(JERRY_TYPES_H):
	$(Q) printf "//\n// Automatically generated: DO NOT EDIT\n//\n\n" > $@
	$(call JerryGetTypes,JERRY_TYPE_,types,JERRY_VALUE_TYPE)
	$(call JerryGetTypes,JERRY_ERROR_,types,JERRY_ERROR_TYPE)
	$(call JerryGetTypes,ERR_,port,JERRY_FATAL_CODE)
	$(call JerryGetTypes,JERRY_OBJECT_TYPE_,types)
	$(call JerryGetTypes,JERRY_FUNCTION_TYPE_,types)
	$(call JerryGetTypes,JERRY_FEATURE_,types)

jerryscript-clean: jerry-clean-types

.PHONY: jerry-clean-types
jerry-clean-types:
	$(Q) rm -f $(JERRY_TYPES_H)
