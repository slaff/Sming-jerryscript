/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Types.cpp
 *
 * @author: Dec 2021 - Mikee47 <mike@sillyhouse.net>
 *
 */

#include "include/Jerryscript/Types.h"
#include "include/jerry_port_vm.h"
#include <debug_progmem.h>

extern "C" {
#include <jerry-core/ecma/base/ecma-helpers.h>
}

// Check Ecma values are correct
#define XX(name, value)                                                                                                \
	static_assert(unsigned(Jerryscript::Ecma::name) == ECMA_##name, "Internal value '" #name "' mismatch");
JERRY_ECMA_MAP(XX)
#undef XX

String toString(Jerryscript::Type type)
{
	switch(type) {
#define XX(jt, t)                                                                                                      \
	case Jerryscript::Type::t:                                                                                         \
		return F(#t);
		JERRY_VALUE_TYPE_MAP(XX)
#undef XX
	default:
		return nullptr;
	}
}

String toString(Jerryscript::ErrorType errorType)
{
	switch(errorType) {
#define XX(jt, t)                                                                                                      \
	case Jerryscript::ErrorType::t:                                                                                    \
		return F(#t);
		JERRY_ERROR_TYPE_MAP(XX)
#undef XX
	default:
		return nullptr;
	}
}

String toString(Jerryscript::ObjectType objectType)
{
	switch(objectType) {
#define XX(jt, t)                                                                                                      \
	case Jerryscript::ObjectType::t:                                                                                   \
		return F(#t);
		JERRY_OBJECT_TYPE_MAP(XX)
#undef XX
	default:
		return nullptr;
	}
}

String toString(Jerryscript::FunctionType functionType)
{
	switch(functionType) {
#define XX(jt, t)                                                                                                      \
	case Jerryscript::FunctionType::t:                                                                                 \
		return F(#t);
		JERRY_FUNCTION_TYPE_MAP(XX)
#undef XX
	default:
		return nullptr;
	}
}

String toString(Jerryscript::Feature feature)
{
	switch(feature) {
#define XX(jt, t)                                                                                                      \
	case Jerryscript::Feature::t:                                                                                      \
		return F(#t);
		JERRY_FEATURE_MAP(XX)
#undef XX
	default:
		return nullptr;
	}
}

namespace Jerryscript
{
Value::Value(int value) : value(ecma_make_int32_value(value))
{
}

Value::Value(unsigned value) : value(ecma_make_uint32_value(value))
{
}

size_t Value::readString(unsigned offset, char* buffer, size_t length) const
{
	return jerry_substring_to_char_buffer(value, offset, offset + length, reinterpret_cast<jerry_char_t*>(buffer),
										  length);
}

String Value::subString(unsigned offset, size_t length) const
{
	String s;
	if(!s.setLength(length)) {
		return nullptr;
	}
	auto len = readString(offset, s.begin(), length);
	s.setLength(len);
	return s;
}

Value::operator String() const
{
	if(isError() || isEmpty()) {
		return nullptr;
	}

	Value strval = toString();
	if(strval.isError()) {
		return nullptr;
	}

	jerry_size_t sz = jerry_get_string_size(strval.get());
	String s;
	if(!s.setLength(sz)) {
		return nullptr;
	}

	jerry_string_to_char_buffer(strval.get(), reinterpret_cast<jerry_char_t*>(s.begin()), sz);

	return s;
}

Array Object::keys() const
{
	return OwnedValue{jerry_get_object_keys(get())};
}

Callable Object::getFunction(const String& name)
{
	Callable func = getProperty(name);
	if(func.isCallable()) {
		return func;
	}

	Error err;
	if(!func.isDefined()) {
		err = Error(ErrorType::Reference, F("Not found"));
	} else if(!func.isError()) {
		err = Error(ErrorType::Type, F("Not Callable"));
	} else {
		err = func;
	}

	debug_e("%s", String(err).c_str());
	return err;
}

static void dbgCheckCall(const String& name, const Value& res)
{
#if DEBUG_BUILD
	if(res.isError()) {
		debug_e("[JS] %s error calling '%s'", String(Error(res)).c_str(), name.c_str());
	}
#endif
}

Value Object::runFunction(const String& name, Value& arg)
{
	Callable func = getFunction(name);
	if(func.isError()) {
		return func;
	}

	auto res = func.call(*this, arg);
	dbgCheckCall(name, res);

	return res;
}

Value Object::runFunction(const String& name, std::initializer_list<Value> args)
{
	Callable func = getFunction(name);
	if(func.isError()) {
		return func;
	}

	auto res = func.call(*this, args);
	dbgCheckCall(name, res);

	return res;
}

Value Error::message() const
{
	Object value = OwnedValue{jerry_get_value_from_error(get(), false)};
	return value[F("message")];
}

Error::operator String() const
{
	if(!isError()) {
		return nullptr;
	}

	String s = ::toString(errorType());
	s += F(" error");
	String msg = message();
	if(msg.length() != 0) {
		s += ": ";
		s += msg;
	}

	return s;
}

Object Value::toObject() const
{
	return OwnedValue{jerry_value_to_object(get())};
}

Value Callable::call(const Object& thisValue, const Value& arg)
{
	jerry_port_watchdog_reset();
	return OwnedValue{jerry_call_function(get(), thisValue.get(), &const_cast<Value&>(arg).get(), 1)};
}

Value Callable::call(const Object& thisValue, std::initializer_list<Value> args)
{
	jerry_port_watchdog_reset();
	return OwnedValue{
		jerry_call_function(get(), thisValue.get(), args.size() ? &args.begin()->get() : nullptr, args.size())};
}

} // namespace Jerryscript
