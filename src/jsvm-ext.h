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

#include "include/Jerryscript/Function.h"

/**
 * External functions that are available to JerryScript
 */

/**
 * @brief Function that prints data in the debug console
 */
JS_DECLARE_FUNCTION(alertFunction)

/**
 * @brief Function that prints data in the standard console
 */
JS_DECLARE_FUNCTION(printFunction)
