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
#define XX(jet, et)                                                                                                    \
	case Jerryscript::ErrorType::et:                                                                                   \
		return F(#et);
		JERRY_ERROR_TYPE_MAP(XX)
#undef XX
	default:
		return nullptr;
	}
}

String toString(Jerryscript::ObjectType objectType)
{
	switch(objectType) {
#define XX(jot, ot)                                                                                                    \
	case Jerryscript::ObjectType::ot:                                                                                  \
		return F(#ot);
		JERRY_OBJECT_TYPE_MAP(XX)
#undef XX
	default:
		return nullptr;
	}
}

String toString(Jerryscript::FunctionType functionType)
{
	switch(functionType) {
#define XX(jft, ft)                                                                                                    \
	case Jerryscript::FunctionType::ft:                                                                                \
		return F(#ft);
		JERRY_FUNCTION_TYPE_MAP(XX)
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
