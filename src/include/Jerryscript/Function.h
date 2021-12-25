/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Function.h
 *
 * @author: Dec 2021 - Mikee47 <mike@sillyhouse.net>
 *
 */

#pragma once

#include "Types.h"

namespace JS
{
using namespace Jerryscript;
}

#define JS_ARGS_0
#define JS_ARGS_1 JS_ARGS_0, &args[0]
#define JS_ARGS_2 JS_ARGS_1, &args[1]
#define JS_ARGS_3 JS_ARGS_2, &args[2]
#define JS_ARGS_4 JS_ARGS_3, &args[3]
#define JS_ARGS_5 JS_ARGS_4, &args[4]
#define JS_ARGS_6 JS_ARGS_5, &args[5]
#define JS_ARGS_7 JS_ARGS_6, &args[6]
#define JS_ARGS_8 JS_ARGS_7, &args[7]

/**
 * @name Macros to implement functions callable from javascript
 *
 * As with all external functions, must be registered using `JS::Object::registerFunction`
 * to make available to javascript.
 *
 * Function arguments should be validated, returning `JS::Error` object on failure.
 *
 * @addtogroup jerryscript
 * @{
 */

/**
 * @brief Argument list is fixed
 * @param func Name of jerryscript wrapper function
 * @param num_args Number of arguments expected by function
 * @param ... Argument definitions
 *
 * Example:
 *
 * ```
 * JS_DEFINE_FUNCTION(myFunction, 2, JS::Value& arg1, JS::Array& arg2)
 * {
 *   ...
 *   return ...;
 * }
 * ```
 *
 */
#define JS_DEFINE_FUNCTION(func, num_args, ...)                                                                        \
	JS::Value js_##func(const JS::CallInfo& callInfo, ##__VA_ARGS__);                                                  \
	jerry_value_t func(const jerry_call_info_t* call_info_p, const jerry_value_t args[],                               \
					   const jerry_length_t args_count)                                                                \
	{                                                                                                                  \
		using Function = JS::Value (*)(const jerry_call_info_t*, ...);                                                 \
		if(num_args != args_count) {                                                                                   \
			return JS::create_arg_count_error(__FUNCTION__);                                                           \
		}                                                                                                              \
		auto res = Function(js_##func)(call_info_p JS_ARGS_##num_args);                                                \
		return res.release();                                                                                          \
	}                                                                                                                  \
	JS::Value js_##func(const Jerryscript::CallInfo& callInfo, ##__VA_ARGS__)

/**
 * @brief Arguments are passed as array
 * @param func Name of jerryscript wrapper function
 *
 * Example:
 *
 * ```
 * JS_DEFINE_FUNCTION_VAR(myFunction)
 * {
 *     for(unsigned i = 0; i < argCount; ++i) {
 * 		 Serial.print(args[i]);
 *     }
 *     return Value();
 * }
 * ```
 *
 */
#define JS_DEFINE_FUNCTION_VAR(func)                                                                                   \
	JS::Value js_##func(const JS::CallInfo& callInfo, JS::Value[], unsigned);                                          \
	jerry_value_t func(const jerry_call_info_t* call_info_p, const jerry_value_t args[],                               \
					   const jerry_length_t args_count)                                                                \
	{                                                                                                                  \
		using Function = JS::Value (*)(const jerry_call_info_t*, JS::Value[], unsigned);                               \
		auto res = Function(js_##func)(call_info_p, (JS::Value*)args, args_count);                                     \
		return res.release();                                                                                          \
	}                                                                                                                  \
	JS::Value js_##func(const JS::CallInfo& callInfo, JS::Value args[], unsigned argCount)

/**
 * @brief Declare a function wrapper
 * @param func Name of wrapper function
 *
 * Use in a header file or as forward declaration
 */
#define JS_DECLARE_FUNCTION(func)                                                                                      \
	jerry_value_t func(const jerry_call_info_t*, const jerry_value_t[], const jerry_length_t);

/** @} */
/** @} */

namespace Jerryscript
{
/**
 * @brief Maps directly onto jerry_call_info_t structure
 */
struct CallInfo {
	Value function;   /**< invoked function object */
	Value this_value; /**< this value passed to the function  */
	Value new_target; /**< current new target value, undefined for non-constructor calls */
};

// Internal helper function used by JS_DEFINE_FUNCTION
jerry_value_t create_arg_count_error(const char* functionName);

} // namespace Jerryscript
