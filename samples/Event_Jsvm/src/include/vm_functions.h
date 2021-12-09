/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * jsvm-ext.cpp
 *
 * @author Nov 2021 - Slavey Karadzhov <slav@attachix.com>
 */

#pragma once

#include <jerryscript.h>
#include <WHashMap.h>
#include <WString.h>

using JsEventData = HashMap<String, String>;

void triggerEvent(const String& name, const JsEventData& data);

/**
 * External functions that are available to JerryScript
 */

/**
 * @brief Function to register event listeners
 * @code {.javascript}
 * 
 * event = {
 *    name => "TEMP_CHANGE",
 *    params = {
 *    },
 *    "origin" => "The\Creator\Of\The\Event" 
 * };
 * 
 * addEventListener("TEMP_CHANGE", function(event) {
 *      console.log("Got Event" + event.name);
 * });
 * 
 * @endcode
 * 
 */
jerry_value_t addEventListener(const jerry_call_info_t* call_info_p, const jerry_value_t args_p[],
							   const jerry_length_t args_count);
