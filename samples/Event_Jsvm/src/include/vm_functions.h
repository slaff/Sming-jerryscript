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

#include <jerry-core/include/jerryscript.h>
#include <WHashMap.h>
#include <WString.h>

#ifdef __cplusplus

using JsEventData = HashMap<String, String>;

void triggerEvent(const String& name, const JsEventData& data);

extern "C" {
#endif

/**
 * External functions that are available to JerryScript
 */

/**
 * @brief Function to register event listeners
 * @code {.javascipt}
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
jerry_value_t addEventListener(const jerry_value_t function_obj, const jerry_value_t this_val,
							   const jerry_value_t args_p[], const jerry_length_t args_cnt);

#ifdef __cplusplus
}
#endif
