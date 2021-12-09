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

#define JS_ARGS_0
#define JS_ARGS_1 JS_ARGS_0, &args[0]
#define JS_ARGS_2 JS_ARGS_1, &args[1]
#define JS_ARGS_3 JS_ARGS_2, &args[2]
#define JS_ARGS_4 JS_ARGS_3, &args[3]
#define JS_ARGS_5 JS_ARGS_4, &args[4]
#define JS_ARGS_6 JS_ARGS_5, &args[5]
#define JS_ARGS_7 JS_ARGS_6, &args[6]
#define JS_ARGS_8 JS_ARGS_7, &args[7]

#define JS_FARGS_0
#define JS_FARGS_1 JS_FARGS_0, const jerry_value_t*
#define JS_FARGS_2 JS_FARGS_1, const jerry_value_t*
#define JS_FARGS_3 JS_FARGS_2, const jerry_value_t*
#define JS_FARGS_4 JS_FARGS_3, const jerry_value_t*
#define JS_FARGS_5 JS_FARGS_4, const jerry_value_t*
#define JS_FARGS_6 JS_FARGS_5, const jerry_value_t*
#define JS_FARGS_7 JS_FARGS_6, const jerry_value_t*
#define JS_FARGS_8 JS_FARGS_7, const jerry_value_t*

// Internal helper function used by JS_DEFINE_FUNCTION
jerry_value_t create_arg_count_error(const char* functionName);

/**
 * @brief Define a function wrapper callable from Javascript, with fixed number of arguments
 * @param func Name of wrapper function
 * @param num_args Number of arguments expected by function
 * 
 * Implementation should have the following prototype:
 * 
 * 		Value myFunction(const CallInfo& callInfo, Value& arg1, Array& arg2, ...)
 *
 * Arguments should be validated, returning `Error` object on failure.
 */
#define JS_DEFINE_FUNCTION(func, num_args)                                                                             \
	jerry_value_t func(const jerry_call_info_t* call_info_p, const jerry_value_t args[],                               \
					   const jerry_length_t args_count)                                                                \
	{                                                                                                                  \
		using namespace ::Jerryscript;                                                                                 \
		using Function = Value (*)(const CallInfo& JS_FARGS_##num_args);                                               \
		if(num_args != args_count) {                                                                                   \
			return create_arg_count_error(__FUNCTION__);                                                               \
		}                                                                                                              \
		auto res = Function(JS::func)(reinterpret_cast<const CallInfo&>(*call_info_p) JS_ARGS_##num_args);             \
		return res.release();                                                                                          \
	}

/**
 * @brief Define a function wrapper callable from Javascript, with variable number of arguments
 * @param func Name of wrapper function
 * 
 * Implementation should have the following prototype:
 * 
 * 		Value myFunction(const CallInfo& callInfo, Value args[], unsigned argCount)
 *
 * Arguments should be validated, returning `Error` object on failure.
 */
#define JS_DEFINE_FUNCTION_VAR(func)                                                                                   \
	jerry_value_t func(const jerry_call_info_t* call_info_p, const jerry_value_t args[],                               \
					   const jerry_length_t args_count)                                                                \
	{                                                                                                                  \
		using namespace ::Jerryscript;                                                                                 \
		using Function = Value (*)(const CallInfo&, Value[], unsigned);                                                \
		auto res = Function(JS::func)(reinterpret_cast<const CallInfo&>(*call_info_p), (Value*)args, args_count);      \
		return res.release();                                                                                          \
	}

/**
 * @brief Declare a function wrapper callable from Javascript
 * @param func Name of wrapper function
 */
#define JS_DECLARE_FUNCTION(func)                                                                                      \
	jerry_value_t func(const jerry_call_info_t*, const jerry_value_t[], const jerry_length_t);

} // namespace Jerryscript
