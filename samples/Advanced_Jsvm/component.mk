COMPONENT_DEPENDS := MultipartParser jerryscript

HWCONFIG := spiffs
DEV_FILES := files/dev
SPIFF_FILES := files/build
APP_JS_SOURCE_DIR := files/js
APP_JS_SNAP_DIR   := $(SPIFF_FILES)

COMPONENT_PREREQUISITES += web-pack

.PHONY: web-pack
web-pack: $(SPIFF_FILES)
	$(Q) cp -r $(APP_JS_SOURCE_DIR)/*.js  $</
	$(Q) cp -r $(DEV_FILES)/* $</ 