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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * External functions that are available to JerryScript
 */

/**
 * brief Function that prints data in the debug console
 */
jerry_value_t alertFunction(const jerry_call_info_t *call_info_p,
							const jerry_value_t args_p[], const jerry_length_t args_count);

/**
 * brief Function that prints data in the standard console
 */
jerry_value_t printFunction(const jerry_call_info_t *call_info_p,
							const jerry_value_t args_p[], const jerry_length_t args_count);

#ifdef __cplusplus
}
#endif
