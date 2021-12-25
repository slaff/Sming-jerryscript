/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Except.h
 *
 * @author: Dec 2021 - Mikee47 <mike@sillyhouse.net>
 *
 */

#include "Types.h"
#include <csetjmp>

namespace Jerryscript
{
enum class FatalCode {
#define XX(jt, t) t = jt,
	JERRY_FATAL_CODE_MAP(XX)
#undef XX
};

class Except
{
public:
	Except() : prev(current)
	{
		current = this;
	}

	~Except()
	{
		current = prev;
	}

	[[noreturn]] static void raise(jerry_fatal_code_t code);

	FatalCode code()
	{
		return mCode;
	}

	operator String() const;

	std::jmp_buf context;

private:
	static Except* current;
	Except* prev;
	FatalCode mCode{};
};

#define JS_TRY()                                                                                                       \
	::Jerryscript::Except e;                                                                                           \
	if(setjmp(e.context) == 0)
#define JS_CATCH() else

} // namespace Jerryscript

String toString(Jerryscript::FatalCode code);
