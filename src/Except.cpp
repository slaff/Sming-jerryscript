/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * ErrorHandler.cpp
 *
 * @author: Dec 2021 - Mikee47 <mike@sillyhouse.net>
 *
 */

#include "include/Jerryscript/VirtualMachine.h"
#include "include/Jerryscript/Except.h"
#include <debug_progmem.h>
#include <csetjmp>

String toString(Jerryscript::FatalCode code)
{
	switch(code) {
#define XX(jt, t)                                                                                                      \
	case Jerryscript::FatalCode::t:                                                                                    \
		return F(#t);
		JERRY_FATAL_CODE_MAP(XX)
#undef XX
	default:
		return String(unsigned(code));
	}
}

namespace Jerryscript
{
Except* Except::current;

void Except::raise(jerry_fatal_code_t code)
{
	if(current == nullptr) {
		debug_e("[JS] Fatal Error %u: calling abort()", code);
		abort();
	}
	current->mCode = FatalCode(code);
	auto& ctx = current->context;
	current = nullptr;
	longjmp(ctx, 1);
}

Except::operator String() const
{
	String s = F("Exception #");
	s += unsigned(mCode);
	s += F(", ");
	s += toString(mCode);
	return s;
}

} // namespace Jerryscript
