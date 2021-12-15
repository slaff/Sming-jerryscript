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

#include "jsvm-ext.h"
#include <m_printf.h>

using namespace Jerryscript;

namespace JS
{
Value alertFunction(const CallInfo& callInfo, Value& str)
{
	String s = str.toString();
	m_nputs(s.c_str(), s.length());
	m_putc('\n');
	return true;
}

Value printFunction(const CallInfo&, Value args[], unsigned argCount)
{
	Value ret_val = Undefined{};

	for(unsigned i = 0; i < argCount; i++) {
		Value str_val = args[i].toString();
		if(str_val.isError()) {
			ret_val = str_val;
			break;
		}

		if(i != 0) {
			m_putc(' ');
		}

		constexpr size_t bufSize{256};
		jerry_length_t pos{0};
		char buffer[bufSize];

		size_t len;
		while((len = str_val.readString(pos, buffer, bufSize)) != 0) {
			m_nputs(buffer, len);
			pos += len;
		}
	}

	m_putc('\n');

	return ret_val;
}

} // namespace JS

JS_DEFINE_FUNCTION(alertFunction, 1)
JS_DEFINE_FUNCTION_VAR(printFunction)
