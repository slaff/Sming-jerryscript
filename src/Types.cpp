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
#include <debug_progmem.h>

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
	if(isError()) {
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
	if(func.isError()) {
		debug_e("[JS] %s error: '%s' not found", String(Error(func)).c_str(), name.c_str());
		return func;
	}

	if(!func.isCallable()) {
		debug_e("[JS] error '%s': not a function", name.c_str());
		return Error(ErrorType::Type, F("Not Callable"));
	}

	return func;
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
	String s = ::toString(errorType());
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

} // namespace Jerryscript
