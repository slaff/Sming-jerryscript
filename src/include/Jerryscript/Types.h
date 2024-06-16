/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Types.h
 *
 * @author: Dec 2021 - Mikee47 <mike@sillyhouse.net>
 *
 */

#pragma once

#include <jerry-core/include/jerryscript.h>
#include ".typemaps.h"
#include <WString.h>
#include <WVector.h>

namespace Jerryscript
{
enum class Type {
#define XX(jt, t) t = jt,
	JERRY_VALUE_TYPE_MAP(XX)
#undef XX
};

enum class ErrorType {
#define XX(jt, t) t = jt,
	JERRY_ERROR_TYPE_MAP(XX)
#undef XX
};

enum class ObjectType {
#define XX(jt, t) t = jt,
	JERRY_OBJECT_TYPE_MAP(XX)
#undef XX
};

enum class FunctionType {
#define XX(jt, t) t = jt,
	JERRY_FUNCTION_TYPE_MAP(XX)
#undef XX
};

enum class Feature {
#define XX(jt, t) t = jt,
	JERRY_FEATURE_MAP(XX)
#undef XX
};

// ECMA values used where simple assignment or check is all that is required
#define JERRY_ECMA_MAP(XX)                                                                                             \
	XX(VALUE_EMPTY, 0x08)                                                                                              \
	XX(VALUE_FALSE, 0x28)                                                                                              \
	XX(VALUE_TRUE, 0x38)                                                                                               \
	XX(VALUE_UNDEFINED, 0x48)                                                                                          \
	XX(VALUE_NULL, 0x58)                                                                                               \
	XX(VALUE_TYPE_MASK, 0x07)                                                                                          \
	XX(TYPE_ERROR, 0x07)                                                                                               \
	XX(TYPE_OBJECT, 0x03)

enum class Ecma {
#define XX(name, value) name = value,
	JERRY_ECMA_MAP(XX)
#undef XX
};

/**
 * @brief Use to initialise Value object by taking ownership of native/raw jerryscript value
 *
 * e.g. `Value myValue = OwnedValue{raw_value};`
 * 
 * This typecast is necessary because `jerry_value_t` is weakly typed (as uint32_t).
 */
struct OwnedValue {
	jerry_value_t value;
};

/**
 * @brief Use to initialise Value object by copying native/raw jerryscript value
 *
 * e.g. `Value myValue = CopyValue{raw_value};`
 */
struct CopyValue {
	jerry_value_t value;
};

/**
 * @brief Use to initialise Value object to a string given a native/raw jerryscript value
 *
 * e.g. `Value myValue = StringValue{raw_value};`
 */
struct StringValue {
	jerry_value_t value;
};

/**
 * @brief Use to create Value containing javascript 'undefined'
 *
 * e.g. `Value myValue = Undefined{};`
 */
struct Undefined {
	Undefined()
	{
	}

	const jerry_value_t value = jerry_value_t(Ecma::VALUE_UNDEFINED);
};

/**
 * @brief Use to create Value containing javascript 'null'
 *
 * e.g. `Value myValue = Null{};`
 */
struct Null {
	Null()
	{
	}

	const jerry_value_t value = jerry_value_t(Ecma::VALUE_NULL);
};

class Object;
class Array;
class Callable;

/**
 * @brief Represents a Jerryscript value
 *
 * This class resembles `std::unique_ptr` to manage a raw/native `jerry_value_t` value.
 * The `get()`, `reset()` and `release()` methods all behave in the same manner.
 *
 * In addition, it provides intrinsic casts from numeric/boolean C++ types plus `String`.
 *
 * You can therefore create a value like this::
 *
 * ```
 * Value value = 12;
 * Value str = "My String";
 * Value str2 = F("My other String");
 * ```
 *
 * If you need to working with floating point values be sure to compile with `JERRY_COMPACT_PROFILE=0`.
 *
 * IMPORTANT: When dealing with raw/native values, **ALWAYS** use either `OwnedValue` or `CopyValue` casts.
 * 
 *   - This is correct:  Value value = OwnedValue{jerry_create_object()};
 *   - This is wrong:    Value value = jerry_create_object(); // Ends up with memory leak plus garbage
 *
 */
class Value
{
public:
	using List = Vector<Value>;

	/**
	 * @brief Construct An empty (unused) value
	 */
	Value()
	{
	}

	/**
	 * @brief Construct a Value and take ownership of the given native value
	 */
	Value(const OwnedValue& value) : value(value.value)
	{
	}

	/**
	 * @brief Construct a Value using a copy (or reference to) the given native value
	 */
	Value(const CopyValue& value) : value(jerry_acquire_value(value.value))
	{
	}

	/**
	 * @brief Construct a string Value from the given native value
	 */
	Value(const StringValue& value) : value(jerry_value_to_string(value.value))
	{
	}

	Value(const Undefined& value) : value(value.value)
	{
	}

	Value(const Null& value) : value(value.value)
	{
	}

	/**
	 * @brief Copy constructor
	 */
	Value(const Value& value) : Value(CopyValue{value.value})
	{
	}

	/**
	 * @brief Move constructor
	 */
	Value(Value&& value)
	{
		reset();
		std::swap(this->value, value.value);
	}

	/**
	 * @name Construct a value from a simple type
	 * @{
	 */

	/**
	 * @brief Integer
	 */
	Value(int value);

	/**
	 * @brief Unsigned integer
	 */
	Value(unsigned value);

	/**
	 * @brief floating-point
	 */
	Value(double value) : value(jerry_create_number(value))
	{
	}

	/**
	 * @brief Boolean
	 */
	Value(bool value) : value(jerry_create_boolean(value))
	{
	}

	/**
	 * @brief Wiring String
	 */
	Value(const String& s) : value(jerry_create_string_sz(reinterpret_cast<const jerry_char_t*>(s.c_str()), s.length()))
	{
	}

	/**
	 * @brief NUL-terminated 'C' string
	 */
	Value(const char* s) : value(jerry_create_string(reinterpret_cast<const jerry_char_t*>(s)))
	{
	}

	/**
	 * @brief Flash String
	 */
	Value(const FSTR::String& s) : Value(String(s))
	{
	}
	/** @} */

	~Value()
	{
		reset();
	}

	/**
	 * @brief Assignment copy operator
	 */
	Value& operator=(const Value& value)
	{
		*this = CopyValue{value.value};
		return *this;
	}

	/**
	 * @brief Assignment move operator
	 */
	Value& operator=(Value&& value)
	{
		if(this != &value) {
			reset();
			std::swap(this->value, value.value);
		}
		return *this;
	}

	/**
	 * @brief Reset contents of object to new value (default is unassigned)
	 */
	Value& reset(jerry_value_t value = jerry_value_t(Ecma::VALUE_EMPTY))
	{
		if(!isEmpty()) {
			jerry_release_value(this->value);
		}
		this->value = value;
		return *this;
	}

	/**
	 * @name Get raw/native value for use with jerryscript C API
	 * @{
	 */

	/**
	 * @brief const get()
	 */
	const jerry_value_t& get() const
	{
		return value;
	}

	/**
	 * @brief get()
	 */
	jerry_value_t& get()
	{
		return value;
	}
	/** @} */

	/**
	 * @brief Get raw/native value and release ownership.
	 */
	jerry_value_t release()
	{
		auto res = value;
		value = jerry_value_t(Ecma::VALUE_EMPTY);
		return res;
	}

	/**
	 * @name Checks on value type
	 * @{
	 */

	/**
	 * @brief Is this object a function? If so, can cast to `Callable` type.
	 */
	bool isCallable() const
	{
		return jerry_value_is_function(value);
	}

	/**
	 * @brief Can this object be accessed as an array? If so, can cast to `Array` type.
	 */
	bool isArray() const
	{
		return jerry_value_is_array(value);
	}

	/**
	 *
	 * @brief Determine if value represents an error. If so, can cast to `Error` type.
	 */
	bool isError() const
	{
		return ecmaType() == Ecma::TYPE_ERROR;
	}

	/**
	 * @brief An empty Value contains nothing, i.e. no javascript type has been assigned.
	 * This gets interpreted as 'undefined' if any attempt is made to use it in javascript.
	 */
	bool isEmpty() const
	{
		return Ecma(value) == Ecma::VALUE_EMPTY;
	}

	/**
	 * @brief Contains a javascript value, but contents undefined
	 */
	bool isDefined() const
	{
		return Ecma(value) != Ecma::VALUE_UNDEFINED;
	}

	/**
	 * @brief A true/false value type
	 */
	bool isBoolean() const
	{
		return jerry_value_is_boolean(value);
	}

	/**
	 * @brief Is this a Boolean type set to False?
	 */
	bool isFalse() const
	{
		return Ecma(value) == Ecma::VALUE_FALSE;
	}

	/**
	 * @brief Is this a Boolean type set to True?
	 */
	bool isTrue() const
	{
		return Ecma(value) == Ecma::VALUE_TRUE;
	}

	/**
	 * @brief Is this a NULL value?
	 */
	bool isNull() const
	{
		return Ecma(value) == Ecma::VALUE_NULL;
	}

	/**
	 * @brief Is this a String?
	 */
	bool isString() const
	{
		return jerry_value_is_string(value);
	}

	/**
	 * @brief Is this an Object type? If so, can cast to `Object` class.
	 */
	bool isObject() const
	{
		return ecmaType() == Ecma::TYPE_OBJECT;
	}

	/**
	 * @brief Does this value contain a Number?
	 */
	bool isNumber() const
	{
		return jerry_value_is_number(value);
	}
	/** @} */

	/**
	 * @brief Get value type
	 */
	Type type() const
	{
		return Type(jerry_value_get_type(value));
	}

	/**
	 * @brief Create a new object from this value
	 */
	Object toObject() const;

	/**
	 * @brief Create a new string value from this value
	 */
	Value toString() const
	{
		return StringValue{value};
	}

	/**
	 * @brief Get content from within a string value
	 * @param offset First character position to read, starting from 0
	 * @param buffer Where to store character data
	 * @param length Number of characters to read
	 * @retval size_t Number of characters read
	 */
	size_t readString(unsigned offset, char* buffer, size_t length) const;

	/**
	 * @brief Get content from within a string value
	 * @param offset First character position to read, starting from 0
	 * @param length Number of characters to read
	 * @retval String Requested range, or nullptr if value is not a string
	 */
	String subString(unsigned offset, size_t length) const;

	/**
	 * @brief Support intrinsic cast to Wiring String
	 */
	operator String() const;

	/**
	 * @brief Used by `as()` method
	 */
	struct As {
		const Value& v;

		operator bool() const
		{
			return v.isTrue();
		}

		operator int() const
		{
			return jerry_value_as_int32(v.get());
		}

		operator unsigned() const
		{
			return jerry_value_as_uint32(v.get());
		}

		operator float() const
		{
			return jerry_get_number_value(v.get());
		}

		operator double() const
		{
			return jerry_get_number_value(v.get());
		}

		operator String() const
		{
			return String(v);
		}
	};

	/**
	 * @brief Get value of object with specific type.
	 * e.g. `value.as<int>()`.
	 *
	 * Return value will revert to sensible default if conversion cannot be completed.
	 * If in doubt, check type first.
	 */
	template <typename T> T as() const
	{
		return T(As{*this});
	}

private:
	Ecma ecmaType() const
	{
		return Ecma(value & unsigned(Ecma::VALUE_TYPE_MASK));
	}

	jerry_value_t value{jerry_value_t(Ecma::VALUE_EMPTY)};
};

/**
 * @brief Object representing an external function implementation
 */
class ExternalFunction : public Value
{
public:
	ExternalFunction(jerry_external_handler_t handler) : Value(OwnedValue{jerry_create_external_function(handler)})
	{
	}
};

/**
 * @brief Objects support named properties
 */
class Object : public Value
{
public:
	using List = Vector<Object>;

	/**
	 * @brief Iterator and operator[] access uses this wrapper class so items may be written or read
	 */
	struct NamedItem {
	public:
		const String& name() const
		{
			return mName;
		}

		operator Value() const
		{
			return mObject.getProperty(mName);
		}

		operator String() const
		{
			return operator Value();
		}

		NamedItem& operator=(const Value& value)
		{
			mObject.setProperty(mName, value);
			return *this;
		}

	private:
		friend class Object;

		NamedItem(Object& object, const String& name) : mObject(object), mName(name)
		{
		}

		Object& mObject;
		String mName;
	};

	using Value::Value;

	/**
	 * @brief Default constructor creates a new, empty object
	 */
	Object() : Value(OwnedValue{jerry_create_object()})
	{
	}

	Object(const Value& value) : Value(value)
	{
	}

	Object(Value&& value) : Value(std::move(value))
	{
	}

	/**
	 * @name Access object properties by name
	 * @{
	 */

	/**
	 * @brief operator[] uses `NamedItem` proxy object so value can be assigned or read
	 */
	NamedItem operator[](const String& name)
	{
		return NamedItem(*this, name);
	}

	/**
	 * @brief const operator[] returns Value directly
	 */
	const Value operator[](const String& name) const
	{
		return getProperty(name);
	}

	/**
	 * @brief Set a property value
	 * @param name Property name
	 * @param value Value to set
	 * @retval Value true on success, otherwise Error
	 */
	Value setProperty(const Value& name, const Value& value)
	{
		return OwnedValue{jerry_set_property(get(), name.get(), value.get())};
	}

	/**
	 * @brief Get a property value
	 * @param name Property name
	 * @retval Value The property value, or Error
	 */
	Value getProperty(const Value& name) const
	{
		return OwnedValue{jerry_get_property(get(), name.get())};
	}

	/**
	 * @brief Determine if a property exists
	 * @param name Property name
	 * @retval bool true if property exists
	 */
	bool hasProperty(const Value& name) const
	{
		return jerry_has_property(get(), name.get());
	}

	/**
	 * @brief Remove a property
	 * @param name Property name
	 * @retval bool true on success
	 */
	bool removeProperty(const Value& name)
	{
		return jerry_delete_property(get(), name.get());
	}
	/** @} */

	/**
	 * @brief Get list of property names
	 */
	Array keys() const;

	/**
	 * @brief Call a specified JavaScript function with exactly one argument
	 * @param name Name of function to run (a property name)
	 * @param arg The argument
	 * @retval Value Return value from function, or Error
	 */
	Value runFunction(const String& name, Value& arg);

	/**
	 * @brief Call a specified JavaScript function with zero or more arguments
	 * @param name Name of function to run (a property name)
	 * @retval Value Return value from function, or Error
	 */
	Value runFunction(const String& name, std::initializer_list<Value> args = {});

	/**
	 * @brief Register an external function so it may be called from javascript
	 * @param name Name of the function (property name)
	 * @param handler The function handler, see Function.h for details
	 * @param bool true on success
	 */
	bool registerFunction(const String& name, jerry_external_handler_t handler)
	{
		auto res = setProperty(name, ExternalFunction(handler));
		return !res.isError();
	}

	/**
	 * @brief Unregister an external function
	 * @param name Name of the function
	 * @param bool true on success
	 */
	bool unregisterFunction(const String& name)
	{
		return removeProperty(name);
	}

	/**
	 * @brief Retrieve the given property as a function
	 * @retval The callable object, or error (property not found or isn't callable)
	 */
	Callable getFunction(const String& name);
};

/**
 * @brief Error object class
 */
class Error : public Value
{
public:
	using Value::Value;

	/**
	 * @name Create an error object
	 * @{
	 */

	/**
	 * @brief Error with type only
	 */
	Error(ErrorType type) : Value(OwnedValue{jerry_create_error_sz(jerry_error_t(type), nullptr, 0)})
	{
	}

	/**
	 * @brief Error with type and message
	 */
	Error(ErrorType type, const String& message)
		: Value(OwnedValue{jerry_create_error_sz(
			  jerry_error_t(type), reinterpret_cast<const jerry_char_t*>(message.c_str()), message.length())})
	{
	}
	/** @} */

	/**
	 * @brief Copy constructor
	 */
	Error(const Value& value) : Value(value)
	{
	}

	/**
	 * @brief Move constructor
	 */
	Error(Value&& value) : Value(std::move(value))
	{
	}

	/**
	 * @brief Get type of error
	 */
	ErrorType errorType() const
	{
		return ErrorType(jerry_get_error_type(get()));
	}

	/**
	 * @brief Get error message, if any
	 */
	Value message() const;

	/**
	 * @brief Formulate error message
	 * Base operator in Value class returns empty value for errors.
	 * To obtain error message, check for `isError()` then cast to Error():
	 * 
	 * ```
	 * if (value.isError()) {
	 *   Serial.println(JS::Error(value));
	 * }
	 * ```
	 */
	operator String() const;
};

/**
 * @brief Provides consistent error message when checking external function arguments
 */
class ArgumentError : public Error
{
public:
	ArgumentError(const String& functionName) : Error(ErrorType::Type, functionName + F(": Invalid arguments"))
	{
	}
};

/**
 * @brief Array objects have properties accessed by index
 */
class Array : public Object
{
public:
	class Iterator;

	/**
	 * @brief Iterator and operator[] access uses this wrapper class so items may be written or read
	 */
	struct IndexedItem {
	public:
		unsigned index() const
		{
			return mIndex;
		}

		operator Value() const
		{
			return mArray.getProperty(mIndex);
		}

		operator String() const
		{
			return operator Value();
		}

		IndexedItem& operator=(const Value& value)
		{
			mArray.setProperty(mIndex, value);
			return *this;
		}

		bool operator==(const IndexedItem& rhs) const
		{
			return &mArray == &rhs.mArray && mIndex == rhs.mIndex;
		}

	private:
		friend class Array;
		friend class Iterator;

		IndexedItem(Array& array, unsigned index) : mArray(array), mIndex(index)
		{
		}

		Array& mArray;
		unsigned mIndex;
	};

	class Iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = IndexedItem;
		using difference_type = std::ptrdiff_t;
		using pointer = IndexedItem*;
		using reference = IndexedItem&;

		Iterator(const Iterator&) = default;

		Iterator(Array& array, unsigned index) : item(array, index)
		{
		}

		Iterator& operator++()
		{
			++item.mIndex;
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator tmp(*this);
			++item.mIndex;
			return tmp;
		}

		Iterator operator+=(size_t distance)
		{
			Iterator tmp(*this);
			item.mIndex += distance;
			return tmp;
		}

		bool operator==(const Iterator& rhs) const
		{
			return item == rhs.item;
		}

		bool operator!=(const Iterator& rhs) const
		{
			return !operator==(rhs);
		}

		IndexedItem operator*()
		{
			return item;
		}

	private:
		IndexedItem item;
	};

	using Object::Object;

	/**
	 * @brief Create a new, fixed-size array with the given number of elements
	 */
	Array(size_t size) : Object(OwnedValue{jerry_create_array(size)})
	{
	}

	/**
	 * @brief Get number of elements in the array
	 */
	size_t count() const
	{
		return jerry_get_array_length(get());
	}

	/**
	 * @name Iterator support
	 * @{
	 */

	/// begin
	Iterator begin()
	{
		return Iterator(*this, 0);
	}

	/// end
	Iterator end()
	{
		return Iterator(*this, count());
	}
	/** @} */

	/**
	 * @name Array element/property access by index
	 * @{
	 */

	/**
	 * @brief operator[] uses `IndexedItem` proxy object so value can be assigned or read
	 */
	IndexedItem operator[](unsigned index)
	{
		return IndexedItem(*this, index);
	}

	/**
	 * @brief const operator[] returns value directly
	 */
	const Value operator[](unsigned index) const
	{
		return getProperty(index);
	}

	/**
	 * @brief Get a property value
	 * @param index Index of property
	 * @retval Value The property value, or Error
	 */
	Value getProperty(unsigned index) const
	{
		return OwnedValue{jerry_get_property_by_index(get(), index)};
	}

	/**
	 * @brief Set a property value
	 * @param index Index of property
	 * @param value Value to set
	 * @retval Value true on success, otherwise Error
	 */
	Value setProperty(unsigned index, const Value& value)
	{
		return OwnedValue{jerry_set_property_by_index(get(), index, value.get())};
	}
	/** @} */
};

/**
 * @brief Callable object represent functions
 */
class Callable : public Object
{
public:
	using List = Vector<Callable>;

	using Object::Object;

	/**
	 * @brief Call with one argument
	 */
	Value call(const Object& thisValue, const Value& arg);

	/**
	 * @brief Call with zero or multiple arguments
	 *
	 * e.g. `call(myObject, {1, 2, 3});`
	 */
	Value call(const Object& thisValue, std::initializer_list<Value> args = {});

	/**
	 * @brief Get specific type of callable object
	 */
	FunctionType functionType() const
	{
		return FunctionType(jerry_function_get_type(get()));
	}
};

/**
 * @brief Get global context
 */
inline Object global()
{
	return OwnedValue{jerry_get_global_object()};
}

} // namespace Jerryscript

String toString(Jerryscript::Type type);
String toString(Jerryscript::ErrorType errorType);
String toString(Jerryscript::ObjectType objectType);
String toString(Jerryscript::FunctionType functionType);
