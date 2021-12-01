COMPONENT_DEPENDS := jerryscript

COMPONENT_SRCDIRS += src
COMPONENT_INCDIRS += $(COMPONENT_SRCDIRS)/include

APP_JS_SOURCE_DIR := files
# APP_JS_SNAP_DIR := out/jerryscript
APP_JS_SNAP_UPDATED := touch app/application.cpp

# We need more heap to run this sample. The default heap is 1Kilobyte, we will use 2K
JERRY_GLOBAL_HEAP_SIZE := 2
