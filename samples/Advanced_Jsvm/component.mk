COMPONENT_DEPENDS := MultipartParser jerryscript

COMPONENT_SRCDIRS += src/
COMPONENT_INCDIRS += $(COMPONENT_SRCDIRS)/include

HWCONFIG := spiffs
DEV_FILES := files/web
SPIFF_FILES := out/web
APP_JS_SOURCE_DIR := files/js
APP_JS_SNAP_DIR   := $(SPIFF_FILES)

COMPONENT_PREREQUISITES += web-build $(SPIFF_FILES)/main.js.snap

DEBUG_VARS += GZIP 
GZIP:= gzip

##@Web Tools

.PHONY: web-build
web-build: $(SPIFF_FILES) ##Create new build
	$(Q) cp $(JERRY_WEB_COMPILER)/* $</
	$(Q) cp -r $(APP_JS_SOURCE_DIR)/*.js  $</
	$(Q) cp -r $(DEV_FILES)/* $</
	$(Q) if command -v $(GZIP) &> /dev/null; then \
			rm $</*.gz; \
			$(GZIP) $</*.html $</jsc.*; \
		fi

.PHONY: web-build-clean	    
web-build-clean:##Clean the build directory
	$(Q) rm -rf $(SPIFF_FILES)
