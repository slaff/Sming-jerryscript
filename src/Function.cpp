/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Function.cpp
 *
 * @author: Dec 2021 - Mikee47 <mike@sillyhouse.net>
 *
 */

#include "include/Jerryscript/Function.h"

namespace Jerryscript
{
jerry_value_t create_arg_count_error(const char* functionName)
{
	return Error(ErrorType::Reference, String(functionName) + F(": wrong number of arguments")).release();
};

} // namespace Jerryscript
