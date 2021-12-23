COMPONENT_DEPENDS := jerryscript

COMPONENT_SRCDIRS += src
COMPONENT_INCDIRS += $(COMPONENT_SRCDIRS)/include

APP_JS_SOURCE_DIR := files
# APP_JS_SNAP_DIR := out/jerryscript
APP_JS_SNAP_UPDATED := touch app/application.cpp

# Enable external contexts
JERRY_EXTERNAL_CONTEXT := 1

DISABLE_NETWORK := 1
