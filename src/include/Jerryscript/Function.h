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

// get number of arguments with JS_NARG
// https://newbedev.com/overloading-macro-on-number-of-arguments
// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#define JS_NARG(...) JS_NARG_I(_0 __VA_OPT__(, ) __VA_ARGS__, JS_RSEQ_N)
#define JS_NARG_I(...) JS_ARG_N(__VA_ARGS__)
#define JS_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define JS_RSEQ_N 8, 7, 6, 5, 4, 3, 2, 1, 0
static_assert(JS_NARG() == 0 && JS_NARG(!, !) == 2, "JS_NARG broken");

#define JS_CONCAT_(x, y) x##y
#define JS_CONCAT(x, y) JS_CONCAT_(x, y)

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
 * @param ... Argument definitions
 *
 * Example:
 *
 * ```
 * JS_DEFINE_FUNCTION(myFunction, JS::Value& arg1, JS::Array& arg2)
 * {
 *   ...
 *   return ...;
 * }
 * ```
 *
 */
#define JS_DEFINE_FUNCTION(func, ...)                                                                                  \
	JS::Value js_##func(const JS::CallInfo& callInfo, ##__VA_ARGS__);                                                  \
	jerry_value_t func(const jerry_call_info_t* call_info_p, const jerry_value_t args[],                               \
					   const jerry_length_t args_count)                                                                \
	{                                                                                                                  \
		using Function = JS::Value (*)(const jerry_call_info_t*, ...);                                                 \
		if(JS_NARG(__VA_ARGS__) != args_count) {                                                                       \
			return JS::create_arg_count_error(__FUNCTION__);                                                           \
		}                                                                                                              \
		auto res = Function(js_##func)(call_info_p JS_CONCAT(JS_ARGS_, JS_NARG(__VA_ARGS__)));                         \
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
