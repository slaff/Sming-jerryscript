/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Context.h - Support for external contexts
 *
 * @author: Dec 2021 - Mikee47 <mike@sillyhouse.net>
 *
 */

#pragma once

#include "Function.h"
#include <memory>

/**
 * @name Macros to implement methods callable from javascript within a Context class
 *
 * Must be used inside a class constructed using `JS::ContextTemplate`.
 *
 * As with all external functions, must be registered using `JS::Object::registerFunction`
 * to make available to javascript.
 *
 * @addtogroup jerryscript
 * @{
 */

/**
 * @brief Argument list is fixed
 * @param method Name of jerryscript wrapper function
 * @param ... Argument definitions
 *
 * Example:
 *
 * ```
 * class MyContext: public JS::ContextTemplate<MyContext>
 * {
 * public:
 *   void init()
 *   {
 *     select();
 *     JS::initialise();
 *     JS::global().registerFunction(F("myMethod"), myMethod);
 *   }
 *
 * protected:
 *   JS_DEFINE_METHOD(myMethod, 2, JS::Value& arg1, JS::Callable& arg2)
 *   {
 *     ...
 *     return ...;
 *   }
 * };
 * ```
 */
#define JS_DEFINE_METHOD(method, ...)                                                                                  \
	static jerry_value_t method(const jerry_call_info_t* call_info_p, const jerry_value_t args[],                      \
								const jerry_length_t args_count)                                                       \
	{                                                                                                                  \
		using Method = JS::Value (ContextClass::*)(const jerry_call_info_t*, ...);                                     \
		if(JS_NARG(__VA_ARGS__) != args_count) {                                                                       \
			return JS::create_arg_count_error(__FUNCTION__);                                                           \
		}                                                                                                              \
		auto& ctx = getCurrent();                                                                                      \
		auto m = reinterpret_cast<Method>(&ContextClass::js_##method);                                                 \
		auto res = (ctx.*m)(call_info_p JS_CONCAT(JS_ARGS_, JS_NARG(__VA_ARGS__)));                                    \
		return res.release();                                                                                          \
	}                                                                                                                  \
	Jerryscript::Value js_##method(const JS::CallInfo& callInfo, ##__VA_ARGS__)

/**
 * @brief Arguments are passed as array
 * @param method Name of jerryscript wrapper function
 *
 * Method declaration has the following prototype:
 *
 * ```
 * JS::Value js_method(const JS::CallInfo& callInfo, JS::Value args[], unsigned argCount)
 * ```
 *
 * Example:
 *
 * ```
 * class MyContext: public JS::ContextTemplate<MyContext>
 * {
 * public:
 *   void init()
 *   {
 *     select();
 *     JS::initialise();
 *     JS::global().registerFunction(F("myMethod"), myMethod);
 *   }
 *
 * protected:
 *   JS_DEFINE_METHOD_VAR(myMethod)
 *   {
 *     for(unsigned i = 0; i < argCount; ++i) {
 * 		 Serial.print(args[i]);
 *     }
 *     return Value();
 *   }
 * };
 * ```
 */
#define JS_DEFINE_METHOD_VAR(method)                                                                                   \
	static jerry_value_t method(const jerry_call_info_t* call_info_p, const jerry_value_t args[],                      \
								const jerry_length_t args_count)                                                       \
	{                                                                                                                  \
		using Method = JS::Value (ContextClass::*)(const jerry_call_info_t*, JS::Value[], unsigned);                   \
		auto& ctx = getCurrent();                                                                                      \
		auto m = reinterpret_cast<Method>(&ContextClass::js_##method);                                                 \
		auto res = (ctx.*m)(call_info_p, (JS::Value*)args, args_count);                                                \
		return res.release();                                                                                          \
	}                                                                                                                  \
	Jerryscript::Value js_##method(const Jerryscript::CallInfo& callInfo, Jerryscript::Value args[], unsigned argCount)

/** @} */
/** @} */

namespace Jerryscript
{
/**
 * @brief Jerryscript external context
 *
 * Applications may be segregated by running in separate contexts.
 * Each context has its own dynamically allocated heap.
 */
class Context
{
public:
	/**
	 * @brief Create a context using the default JERRY_GLOBAL_HEAP_SIZE setting
	 */
	Context();

	/**
	 * @brief Create a context with custom heap size
	 * @param heapSize Size of heap in bytes. Will be rounded up as necessary.
	 */
	Context(size_t heapSize);

	~Context()
	{
		if(current == this) {
			current = nullptr;
		}
	}

	explicit operator bool() const
	{
		return bool(context);
	}

	/**
     * @brief Make this the current context
     *
     * Context must be selected before calling into jerryscript.
     */
	void select()
	{
		current = this;
	}

	operator jerry_context_t*() const
	{
		return reinterpret_cast<jerry_context_t*>(context.get());
	}

	static Context& getCurrent()
	{
		assert(current != nullptr);
		return *current;
	}

private:
	static void* alloc(size_t size, void* param);

	std::unique_ptr<uint8_t[]> context;
	static Context* current;
};

/**
 * @brief Implement a custom Context class
 */
template <class ClassType> class ContextTemplate : public Context
{
public:
	using ContextClass = ClassType;

	using Context::Context;

	static ClassType& getCurrent()
	{
		return static_cast<ClassType&>(Context::getCurrent());
	}
};

} // namespace Jerryscript
