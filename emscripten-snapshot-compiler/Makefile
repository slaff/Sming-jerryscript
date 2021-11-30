#
# Build emscripten snapshot compiler
#

PYTHON ?= python3
JERRYSCRIPT_ROOT ?= $(abspath $(CURDIR)/../jerryscript)
BUILD_DIR := $(CURDIR)/build

# emscripten WebAssembly toolchain
# https://emscripten.org/index.html
ifdef EMSDK
export EMSCRIPTEN ?= $(EMSDK)/upstream/emscripten
endif

ifndef EMSCRIPTEN
$(error 'EMSCRIPTEN' undefined - please configure emsdk)
endif

EMSCRIPTEN_TOOLCHAIN := $(EMSCRIPTEN)/cmake/Modules/Platform/Emscripten.cmake

# Options
JERRY_PROFILE ?= minimal

# Build targets
.PHONY: all
all: build-default

.PHONY: build-default
build-default:
	$(MAKE) build JERRY_PROFILE=minimal
	$(MAKE) build JERRY_PROFILE=es.next

# Build compiler
.PHONY: build
build:
	$(PYTHON) $(JERRYSCRIPT_ROOT)/tools/build.py \
		--toolchain="$(EMSCRIPTEN_TOOLCHAIN)" \
		--jerry-cmdline-snapshot ON \
		--error-messages ON \
		--lto OFF \
		--strip OFF \
		--profile $(JERRY_PROFILE) \
		--compile-flag "-Wno-implicit-int-float-conversion " \
		--builddir $(BUILD_DIR)/jerryscript
	mkdir -p $(BUILD_DIR)
	cmake --verbose -DCMAKE_BUILD_TYPE=MinSizeRel -DJERRY_PROFILE=$(JERRY_PROFILE) -S . -B $(BUILD_DIR)
	$(MAKE) -C $(BUILD_DIR)

# Clean build directory
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
